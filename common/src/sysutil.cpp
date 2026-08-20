#include "sysutil.h"
#include "strutil.h"
#include "timeutil.h"
#include "fileutil.h"

#include <string.h>
#include <stdlib.h>

#if defined(WIN32) || defined(_WIN32_WCE)
#include <ctime>
#include <Windows.h>
#else
#include <sys/time.h>
#endif

using namespace std;
using namespace comm;

void SysUtil::getCmdLineArgs(int argc_i, char** argv_i, int& argc_r, char**& argv_r)
{
	int i=0;
	StringArray argv_array_o;
	StringArray argv_array;

#if defined(WIN32) || defined(_WIN32_WCE)
	LPWSTR cmdLineW= GetCommandLineW();
	string cmdLine = StrUtil::Unicode2Mbcs(cmdLineW);
    StrUtil::spilt(cmdLine, ' ', argv_array_o);
#else
	for(i=0; i<argc_i; i++)
	{
		std::string tep = argv_i[i];
		argv_array_o.push_back(tep);
	}
#endif
	
	//带""的中间可以有空格，算一个参数
	for (i=0; i<(int)argv_array_o.size(); i++)
	{
		string param = argv_array_o[i];
		if (StrUtil::beginWith(param, "\""))
		{
			if (!StrUtil::endWith(param, "\""))
			{
				i++;
				for (; i<(int)argv_array_o.size(); i++)
				{
					if (StrUtil::endWith(argv_array_o[i], "\""))
					{
						param = param + " " + argv_array_o[i];
						break;
					}
					else
					{
						param = param + " " + argv_array_o[i];
					}
				}
			}
		}

		StrUtil::removeBeginStr(param, "\"");
		StrUtil::removeEndStr(param, "\"");
		argv_array.push_back(param);
	}

	for (i=0; i<(int)argv_array.size(); i++)
	{
		if (StrUtil::beginWith(argv_array[i], "-"))
		{
			break;
		}
	}

	argc_r = 0;
	argv_r = NULL;

	if ((int)argv_array.size() > i)
	{
		argv_r = new char*[argv_array.size() - i];
		for (; i<(int)argv_array.size(); i++)
		{
			int len = (int)(argv_array[i].length());
			argv_r[argc_r] = new char[len + 1];
			strcpy(argv_r[argc_r], argv_array[i].c_str());
			argc_r++;
		}
	}
}

void SysUtil::releaseCmdLineArgs(int argc_r, char** argv_r)
{
	for (int i=0; i<argc_r; i++)
	{
		delete argv_r[i];
	}
	delete argv_r;
}

int SysUtil::getRandom(int n)
{
	static int call_times = 0;
	call_times = (++call_times) % 30; //prevent called frequently

	struct timeval current;
	TimeUtil::gettimeofday(&current, NULL);
	srand(current.tv_sec * 1000 + current.tv_usec * call_times);
	
	int x = rand() % n;
	return x;
}

bool SysUtil::setEnv(const char* key, const char* value)
{
    bool isSuccessed = false;

#if defined(WIN32) || defined(_WIN32_WCE)
    int ret = ::SetEnvironmentVariableA(key, value);
    if (ret != 0)
        isSuccessed = true;
#else
    int ret = setenv(key, value, 1);
    if (ret == 0)
        isSuccessed = true;
#endif

    return isSuccessed;
}

string SysUtil::getEnv(const char* key)
{
    return getEnvStr(key, "");
}

std::string SysUtil::getEnvStr(const char* key, const std::string& defValue)
{
	string strenv = defValue;

#if defined(WIN32) || defined(_WIN32_WCE)
	char szEnv[300];
	memset(szEnv, 0x00, sizeof(szEnv));

	int len = ::GetEnvironmentVariableA(key, szEnv, sizeof(szEnv));
	strenv = szEnv;
#else
	char* env = getenv(key);
	if (env != NULL)
		strenv = env;
#endif

	return strenv;
}

bool SysUtil::getEnvBool(const char* key, bool defValue)
{
	bool ret_value = defValue;

	string str_value = SysUtil::getEnv(key);
	if (str_value != "")
	{
		if (str_value == "0")
		{
			ret_value = false;
		}
		else
		{
			ret_value = true;
		}
	}

	return ret_value;
}

int SysUtil::getEnvInt(const char* key, int defValue)
{
	int ret_value = defValue;

	string str_value = SysUtil::getEnv(key);
	if (str_value != "")
	{
		ret_value = StrUtil::Str2Int(str_value);
	}

	return ret_value;
}

double SysUtil::getEnvDbl(const char* key, double defValue)
{
	double ret_value = defValue;

	string str_value = SysUtil::getEnv(key);
	if (str_value != "")
	{
		ret_value = StrUtil::Str2Dbl(str_value);
	}

	return ret_value;
}

//13317410.99 52284321.84
int SysUtil::getSysUpSeconds()
{
	string content = FileUtil::file_read("/proc/uptime");
	//string content = "13317410.99 52284321.84";
	if (content.empty())
	{
		return -1;
	}

	StringArray elems;
	StrUtil::spilt(content, " ", elems);
	if (elems.size() < 2)
	{
		return -1;
	}

	return StrUtil::Str2Int(elems[0]);
}
