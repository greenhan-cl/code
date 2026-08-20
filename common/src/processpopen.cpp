#include "platinc.h"
#include<stdlib.h>
#include<string.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <limits.h>
#include "processpopen.h"
#include "pathutil.h"
#include "strutil.h"
#include "sysutil.h"

using namespace std;
using namespace comm;

#ifdef  OPEN_MAX
static long openmax = OPEN_MAX;
#else
static long openmax = 0;
#endif
 
/*
 * If OPEN_MAX is indeterminate, we're not
 * guaranteed that this is adequate.
 */
#define OPEN_MAX_GUESS 256
 
long
open_max(void)
{
    if (openmax == 0) {      /* first time through */
        errno = 0;
        if ((openmax = sysconf(_SC_OPEN_MAX)) < 0) {
           if (errno == 0)
               openmax = OPEN_MAX_GUESS;    /* it's indeterminate */
           else
               perror("sysconf error for _SC_OPEN_MAX");
        }
    }
 
    return(openmax);
}


static pid_t *childpid = NULL;
/* ptr to array allocated at run-time */
static int maxfd; /* from our open_max(), {Prog openmax} */

void my_exec(const char* cmdstring)
{
	string cmd_str = cmdstring;
	StrUtil::trim(cmd_str);
	
	StringArray args_array;
	StrUtil::spilt2(cmd_str, ' ', args_array);
	if (args_array.empty())
	{
		return;
	}

	string exe_path_name = args_array.at(0);
	string exe_name = PathUtil::getFilenameOfFullFilename(exe_path_name);
	bool has_path = false;
	if (StrUtil::hasContain(exe_path_name, "/"))
	{
		has_path = true;
	}
	args_array[0] = exe_name; //replace with nopath

	int argc_r = args_array.size();
	char** argv_r = new char*[argc_r+1];
	argv_r[args_array.size()] = (char*)NULL;

	for (int j=0; j<(int)args_array.size(); j++)
	{
		int len = (int)(args_array[j].length());
		argv_r[j] = new char[len+1];
		strcpy(argv_r[j], args_array[j].c_str());
		//printf("argv_r[%d]=%s\n", j, argv_r[j]);  //can't print, will affect popen
	}

	int ret = 0;
	if (has_path)
	{
		//printf("execv %s\n", exe_path_name.c_str()); //can't print, will affect popen
		ret = execv(exe_path_name.c_str(), argv_r); 
	}
	else
	{
		//printf("execvp %s\n", exe_path_name.c_str()); //can't print, will affect popen
		ret = execvp(exe_name.c_str(), argv_r);
	}

	//printf("exec ret=%d errno=%d(%s)", ret, errno, strerror(errno)); //can't be here
	SysUtil::releaseCmdLineArgs(argc_r, argv_r);
}

int my_system(const char* cmdstring)
{
	pid_t pid; 
	int status; 

	if(cmdstring == NULL) 
	{ 
		return (1);
	} 

	if((pid = fork())<0) 
	{ 
		status = -1;
	} 
	else if (pid == 0) 
	{
		my_exec(cmdstring);
		_exit(127); //if success will not here
	}
	else
	{
		while(waitpid(pid, &status, 0) < 0) 
		{ 
			if(errno != EINTR) 
			{ 
				status = -1;
				break; 
			} 
		}
	}
	return status; //return childpid's retvalue if success
}


int my_system_nowait(const char* cmdstring)
{
	pid_t pid; 
	int status; 

	if(cmdstring == NULL) 
	{ 
		return (1);
	} 

	if((pid = fork())<0) 
	{ 
		status = -1;
	} 
	else if (pid == 0) 
	{
		my_exec(cmdstring);
		_exit(127); //if success will not here
	}
	else
	{
		status = 0;
	}
	return status;
}

FILE *
my_popen(const char *cmdstring, const char *type)
{
	int i, pfd[2];
	pid_t pid;
	FILE *fp;

	/* only allow "r" or "w" */
	if ((type[0] != 'r' && type[0] != 'w') || type[1] != 0) {
		errno = EINVAL; /* required by POSIX.2 */
		return(NULL);
	}

	if (childpid == NULL) { /* first time through */
		/* allocate zeroed out array for child pids */
		maxfd = open_max();
		if ( (childpid = (pid_t*) calloc(maxfd, sizeof(pid_t))) == NULL)
			return(NULL);
	}

	if (pipe(pfd) < 0)
		return(NULL); /* errno set by pipe() */

	if ( (pid = fork()) < 0)
		return(NULL); /* errno set by fork() */
	else if (pid == 0) { /* child */
		if (*type == 'r') {
			close(pfd[0]);
			if (pfd[1] != STDOUT_FILENO) {
				dup2(pfd[1], STDOUT_FILENO);
				close(pfd[1]);
			}
		} else {
			close(pfd[1]);
			if (pfd[0] != STDIN_FILENO) {
				dup2(pfd[0], STDIN_FILENO);
				close(pfd[0]);
			}
		}
		/* close all descriptors in childpid[] */
		for (i = 0; i < maxfd; i++)
			if (childpid[ i ] > 0)
				close(i);

		//execl(SHELL, "sh", "-c", cmdstring, (char *) 0);
		my_exec(cmdstring); 

		_exit(127);
	}
	/* parent */
	if (*type == 'r') {
		close(pfd[1]);
		if ( (fp = fdopen(pfd[0], type)) == NULL)
			return(NULL);
	} else {
		close(pfd[0]);
		if ( (fp = fdopen(pfd[1], type)) == NULL)
			return(NULL);
	}
	childpid[fileno(fp)] = pid; /* remember child pid for this fd */
	return(fp);
}

int
my_pclose(FILE *fp)
{
	int fd, stat;
	pid_t pid;

	if (childpid == NULL)
		return(-1); /* popen() has never been called */

	fd = fileno(fp);
	if ( (pid = childpid[fd]) == 0)
		return(-1); /* fp wasn't opened by popen() */

	childpid[fd] = 0;
	if (fclose(fp) == EOF)
		return(-1);

	while (waitpid(pid, &stat, 0) < 0)
	{
		if (errno != EINTR)
			return(-1); /* error other than EINTR from waitpid() */
	}

	return(stat); /* return child's termination status */
}

int 
my_childpid(FILE* fp)
{
	pid_t pid;

	if (childpid == NULL)
		return(-1); /* popen() has never been called */

	int fd = fileno(fp);
	if ( (pid = childpid[fd]) == 0)
		return(-1); /* fp wasn't opened by popen() */

	return (int)pid;
}
