#ifndef PROCESS_POPEN_H__
#define PROCESS_POPEN_H__

//only used by _HWRT__
int my_system(const char* cmdstring);
int my_system_nowait(const char* cmdstring);
FILE* my_popen(const char *cmdstring, const char *type);
int my_pclose(FILE *fp);
int my_childpid(FILE* fp);

#endif
