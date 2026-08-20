#include "processhelper.h"
#include "strutil.h"
#include "timeoutrunner.h"
#include <chrono>
#include <thread>

#ifdef _WIN32
#include <TlHelp32.h>
#define popen _popen
#define pclose _pclose
#else
#include<stdlib.h>
#endif

#if defined(__USE_MYPOPEN__)
#include "processpopen.h"
#endif

using namespace std;
using namespace comm;

ProcessHelper::ProcessHelper(Logger* logger)
{
	m_logger = logger;
}

int ProcessHelper::system_run(const string& cmd){
	if (m_logger != NULL) {
		m_logger->log(cmd);
	}
#if defined(__USE_MYPOPEN__)
	int ret = my_system(cmd.c_str());
#else
	int ret = system(cmd.c_str());
#endif

	if (ret != 0)
	{
		if (m_logger != NULL)
			m_logger->logFmt("system ret=%d errno=%d(%s)", ret, errno, strerror(errno));
	}

	return ret;
}

int ProcessHelper::system_run_nowait(const string& cmd){
	if (m_logger != NULL)
		m_logger->log(cmd);
#if defined(__USE_MYPOPEN__)
	int ret = my_system_nowait(cmd.c_str());
#else
	int ret = 0; //not implemention
#endif

	if (ret != 0)
	{
		if (m_logger != NULL)
			m_logger->logFmt("system ret=%d errno=%d(%s)", ret, errno, strerror(errno));
	}
	return ret;
}

#ifdef _WIN32
int ProcessHelper::GetProcessCount(const char* processName)
{
	HANDLE hSnapshot;
	PROCESSENTRY32 lppe;
	BOOL found;
	int count = 0;
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	lppe.dwSize = sizeof(PROCESSENTRY32);
	found=Process32First(hSnapshot,&lppe);

	while(found)
	{
		string exe_name = StrUtil::Unicode2Mbcs(lppe.szExeFile);
		if(exe_name == processName)
		{
			count++;
		}
		found = Process32Next(hSnapshot,&lppe);
	}
	CloseHandle(hSnapshot);
	return count;
}

void ProcessHelper::KillProcessByName(const char* processName)
{
	HANDLE hSnapshot;
	PROCESSENTRY32 lppe;
	BOOL found;
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	lppe.dwSize = sizeof(PROCESSENTRY32);
	found=Process32First(hSnapshot,&lppe);
	if (!found)
	{
		CloseHandle(hSnapshot);
		return;
	}

	while(found)
	{
		string exe_name = StrUtil::Unicode2Mbcs(lppe.szExeFile);
		if(exe_name == processName)
		{
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, lppe.th32ProcessID);
			if (hProcess)
			{
				if (TerminateProcess(hProcess,0)){
					if(m_logger!=NULL) m_logger->logFmt("TerminateProcess %s success", processName);
				}
				else {
					if(m_logger!=NULL) m_logger->logFmt("TerminateProcess %s Failed", processName);
				}
				CloseHandle(hProcess);
			}
		}
		found = Process32Next(hSnapshot,&lppe);
	}
	CloseHandle(hSnapshot);
}

void ProcessHelper::KillProcessById(int pid)
{
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
	if (hProcess)
	{
		if (TerminateProcess(hProcess,0)){
			if(m_logger!=NULL) m_logger->logFmt("TerminateProcess %d success", pid);
		}
		else {
			if(m_logger!=NULL) m_logger->logFmt("TerminateProcess %d Failed", pid);
		}
		CloseHandle(hProcess);
	}	
}
#else
int ProcessHelper::GetProcessCount(const char* processName)
{
	//ps -ef |grep processName
	return 0;
}
void ProcessHelper::KillProcessById(int pid)
{
	string cmd = string("kill -9 ") + StrUtil::Int2Str(pid);	
	system_run(cmd);
}
void ProcessHelper::KillProcessByName(const char* processName)
{
	string cmd = string("killall -9 ") + processName;	
	cmd += " 2>/dev/null";
	system_run(cmd);
}

void ProcessHelper::KillProcessByNameSU(const char* processName)
{
	string cmd = string("killall -9 ") + processName;	

#if defined(__LINUX__) && !defined(ANDROID)
	cmd = "sudo " + cmd;
#endif

	cmd += " 2>/dev/null";
	system_run(cmd);
}
#endif

 int ProcessHelper::runShell(const char* cmd, char* data, int len) {
	//m_logger->logFmt("%s", cmd);

#if defined(__USE_MYPOPEN__)
	FILE* pipe = my_popen(cmd, "r");
#else
	FILE* pipe = popen(cmd, "r");
#endif

	if (!pipe) {
		if(m_logger!=NULL) {
			m_logger->logFmt("popen failed. errno=%d(%s)", errno, strerror(errno));
		}
		return -1;
	}

	int count = fread(data, sizeof(char), len, pipe);
	if(m_logger!=NULL){
		m_logger->logFmt("%s, out_size=%d", cmd, count);
	}

#if defined(__USE_MYPOPEN__)
	my_pclose(pipe);
#else
	pclose(pipe);
#endif
	return count;
}

 bool ProcessHelper::runShell(const char* cmd, StringArray& dataArray) {
	//m_logger->logFmt("%s", cmd);

#if defined(__USE_MYPOPEN__)
	FILE* pipe = my_popen(cmd, "r");
#else
	FILE* pipe = popen(cmd, "r");
#endif

	if (!pipe) {
		if (m_logger != NULL)
			m_logger->logFmt("popen failed. errno=%d(%s)", errno, strerror(errno));
		return false;
	}

	char buff[1024];
	while (!feof(pipe) && !ferror(pipe)) {
		strcpy(buff, "\0"); 
		if (fgets(buff, sizeof(buff), pipe) != NULL)
		{
			dataArray.push_back(buff);
		}
	}

	if(m_logger!=NULL){
		m_logger->logFmt("%s, out_lines=%d", cmd, dataArray.size());
	}

#if defined(__USE_MYPOPEN__)
	my_pclose(pipe);
#else
	pclose(pipe);
#endif
	return true;
}

 std::string ProcessHelper::runShell(const char* cmd)
 {
	 char data[10 * 1024];
	 memset(data, 0x00, sizeof(data));

	 int ret = runShell(cmd, data, sizeof(data)-1);
	 if (ret < 0)
	 {
		 return "";
	 }
	 return data;
 }

 bool ProcessHelper::runShellTrim(const char* cmd, StringArray& dataArray) {
	bool ret = runShell(cmd, dataArray);
	if (ret)
	{
		for(int i=0; i<(int)dataArray.size(); i++){
			StrUtil::trim(dataArray[i]);
		}
	}
	return ret;
 }

 std::string ProcessHelper::runShellTrim(const std::string& cmd)
 {
	 string line = runShell(cmd.c_str());
	 StrUtil::trim(line);
	 return line;
 }

 std::string ProcessHelper::runShellTrim(const char* cmd)
 {
	string line = runShell(cmd);
	StrUtil::trim(line);
	return line;
 }

bool ProcessHelper::runShellTimeout(const char* cmd, StringArray& dataArray, int timeout_ms, int tries, const std::string& cmd_if_timeout) 
{
	for (int i=0; i<tries; i++)
	{
		string timeout_cmd = cmd_if_timeout;
#if defined(__USE_MYPOPEN__)
		FILE* pipe = my_popen(cmd, "r");

		int pipe_pid = my_childpid(pipe);
		if (pipe_pid > 0) {
			timeout_cmd = "kill -9 " + StrUtil::Int2Str(pipe_pid);
		}
#else
		FILE* pipe = popen(cmd, "r");
#endif

		if (!pipe) {
			if(m_logger!=NULL)
				m_logger->logFmt("popen failed. errno=%d(%s)", errno, strerror(errno));
			return false;
		}

		TimeoutRunner timeout_worker(timeout_ms, timeout_cmd, m_logger);
		timeout_worker.start();

		//if(m_logger!=NULL)
		//	m_logger->logFmt("feof(pipe)=%d ferror(pipe)=%d", feof(pipe), ferror(pipe));
		char buff[1024];
		while (!feof(pipe) && !ferror(pipe)) {
			strcpy(buff, "\0"); 
			if (fgets(buff, sizeof(buff), pipe) != NULL)
			{
				dataArray.push_back(buff);
			}
		}

		if(m_logger!=NULL)
			m_logger->logFmt("%s, out_lines=%d", cmd, dataArray.size());

		if (timeout_worker.hasTimeout())
		{
			if (timeout_cmd != "")
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(500)); //just wait timeout_worker run finish. otherwise timeout_worker will release when the method finish.(crash)
			}

			if(m_logger!=NULL)
				m_logger->log("hasTimeout");
		#if defined(__USE_MYPOPEN__)
			my_pclose(pipe);
		#else
			pclose(pipe);
		#endif
			continue; //need retry
		}
		else
		{
			timeout_worker.stop();
		#if defined(__USE_MYPOPEN__)
			my_pclose(pipe);
		#else
			pclose(pipe);
		#endif
			return true;
		}
	}

	if(m_logger!=NULL)
		m_logger->log("runShellTimeout failed");
	return false;
}

