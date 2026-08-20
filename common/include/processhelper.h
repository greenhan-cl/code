#ifndef PROCESS_HELPER_H__
#define PROCESS_HELPER_H__

#include "logger.h"
#include "strutil.h"

namespace comm
{

class ProcessHelper
{
public:
	ProcessHelper(Logger* logger);

	//will run proc and wait it return. call system() inner.
	int system_run(const std::string& cmd);
	int system_run_nowait(const std::string& cmd);
	int GetProcessCount(const char* processName);
	void KillProcessById(int pid);
	void KillProcessByName(const char* processName);
	void KillProcessByNameSU(const char* processName);

	int runShell(const char* cmd, char* data, int len);	//-1 fail
	bool runShell(const char* cmd, StringArray& dataArray);
	//output max 10KB
	std::string runShell(const char* cmd);

	//should use this 3 methods always
	std::string runShellTrim(const std::string& cmd);
	std::string runShellTrim(const char* cmd);
	bool runShellTrim(const char* cmd, StringArray& dataArray);

	bool runShellTimeout(const char* cmd, StringArray& dataArray, int timeout_ms, int tries, const std::string& cmd_if_timeout);

private:
	comm::Logger* m_logger;
};

}

#endif
