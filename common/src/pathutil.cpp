#include "pathutil.h"
#include "strutil.h"
#include "sysutil.h"
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include "platinc.h"
#include "devicehelper.h"

#ifdef __LINUX__
#include <dirent.h> 
#else
#include <direct.h>
#include <io.h>
#endif

#if defined(WIN32)
//nothing
#else
	#include <dirent.h>
	#include <sys/stat.h>
#endif

using namespace std;
using namespace comm;

std::string PathUtil::GetExePathA()
{
#if defined(WIN32)
    std::string curPath = "";
    curPath = SysUtil::getEnv(DT_EXEPATH);

    if (curPath == "")
    {
	    char szCurPath[MAX_PATH];
	    memset(szCurPath, 0x00, sizeof(szCurPath));

	    HMODULE hModule = ::GetModuleHandleA(NULL);
	    if (hModule != NULL)
	    {
		    ::GetModuleFileNameA(hModule, szCurPath, MAX_PATH);
	    }

        curPath = szCurPath;
    }

	int pos = (int)curPath.rfind('\\');
	curPath = curPath.substr(0, pos + 1);
	return curPath;

#elif defined(__IPHONEOS__)
	//use env DT_EXEPATH instead , as not find the way of get curexepath
	//if not set DT_EXEPATH, then use ./
    std::string curPath = "";
    curPath = SysUtil::getEnv(DT_EXEPATH);

    if (curPath == "")
    {
        curPath = "./";
    }

    int pos = (int)curPath.rfind('/');
    curPath = curPath.substr(0, pos + 1);

    return curPath;

#else
    std::string curPath = "";
    curPath = SysUtil::getEnv(DT_EXEPATH);

    if (curPath == "")
    {
        char szCurPath[260];
        memset(szCurPath, 0x00, sizeof(szCurPath));
        int rslt = readlink("/proc/self/exe", szCurPath, sizeof(szCurPath));
        if(rslt <0 || rslt>=260)
            return curPath;
        curPath = szCurPath;
    }

    int pos = (int)curPath.rfind('/');
    curPath = curPath.substr(0, pos + 1);

    return curPath;

#endif
}

std::string PathUtil::GetIniPath()
{
	std::string curPath = "";
    curPath = SysUtil::getEnv(DT_INIPATH);

	if (curPath != "")
	{
		int pos = (int)curPath.rfind(PATH_SEPA);
		curPath = curPath.substr(0, pos + 1);
	}
	else
	{
		curPath = GetExePathA();
	}

	return curPath;
}

std::string PathUtil::GetLogPath()
{
	return DeviceHelper::getLogPath();
}

std::wstring PathUtil::GetExePathW()
{
#if defined(WIN32) || defined(_WIN32_WCE)
    std::wstring curPath = L"";
    curPath = StrUtil::Mbcs2Unicode(SysUtil::getEnv(DT_EXEPATH));

    if (curPath == L"")
    {
	    wchar_t szCurPath[MAX_PATH];
	    memset(szCurPath, 0x00, sizeof(szCurPath));

	    HMODULE hModule = ::GetModuleHandleW(NULL);
	    if (hModule != NULL)
	    {
		    ::GetModuleFileNameW(hModule, szCurPath, MAX_PATH);
	    }

	    curPath = szCurPath;
    }

	int pos = (int)curPath.rfind(L'\\');
	curPath = curPath.substr(0, pos + 1);
	return curPath;

#elif defined(ANDROID)
	return L"";
#else
	char szCurPath[260];
	std::wstring curPath =L"";
	memset(szCurPath, 0x00, 256);

    std::string envPath = SysUtil::getEnv(DT_EXEPATH);
    if (envPath != "")
    {
        strcpy(szCurPath, envPath.c_str());
    }
    else
    {
	    int rslt = readlink("/proc/self/exe", szCurPath, 260);
	    if(rslt <0 || rslt>=260)
            return curPath;
    }

	int length = mbstowcs(NULL, szCurPath, 0);
	wchar_t* pWideStr = new wchar_t[length + 1];
	memset(pWideStr, 0, sizeof(wchar_t) * (length + 1));

	mbstowcs(pWideStr, szCurPath, length);
	curPath = pWideStr;
	delete[] pWideStr;
	return curPath;
#endif
}

bool PathUtil::PathExist(const std::string& path)
{
#if defined(WIN32) || defined(_WIN32_WCE)

	string path1 = path;
	if (!StrUtil::endWith(path1, "\\"))
	{
		path1 += "\\";
	}
	
	bool ret = false;
	int attr = 0;
	attr = GetFileAttributesW(StrUtil::Mbcs2Unicode(path1).c_str());
	if (attr!=-1 && (attr&FILE_ATTRIBUTE_DIRECTORY))
	{
		ret = true;
	}

	return ret;

#elif defined(__IPHONEOS__)
	string path1 = path;
	if(!StrUtil::endWith(path1, "/"))
	{
		path1 += "/";
	}

	bool ret = false;
	if(access(path1.c_str(), F_OK) == 0)
	{
		ret = true;
	}
	return ret;

#else
	string path1 = path;
	if(!StrUtil::endWith(path1, "/"))
	{
		path1 += "/";
	}

	bool ret = false;
	DIR* dir = NULL;
	dir = opendir(path1.c_str());
	if(dir)
	{
		closedir(dir);
		ret = true;
	}

	return ret;
#endif
}

bool PathUtil::PathCreate(const std::string& path)
{
#if defined(WIN32) || defined(_WIN32_WCE)
	SECURITY_ATTRIBUTES sa;   
	sa.nLength=sizeof(SECURITY_ATTRIBUTES);   
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = 0;
	bool ret = false;
	if(CreateDirectoryW(StrUtil::Mbcs2Unicode(path).c_str(), &sa))
	{
		ret = true;
	}
	return ret;
#else
	bool ret = false;
	int attr = -1;
	attr = mkdir(path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
	if(attr != -1)ret = true;
	return ret;
#endif
}

std::string PathUtil::getPathOfFullFilename(const std::string& fullFileName)
{
	if(fullFileName.length() == 0)
		return "";
    int index = fullFileName.find_last_of("\\/");
    if (index == string::npos)
        return "";

    return fullFileName.substr(0, index+1);
}

std::string PathUtil::getFilenameOfFullFilename(const std::string& fullFileName)
{
	if(fullFileName.length() == 0)
		return "";
    int index = fullFileName.find_last_of("\\/");
    if (index == string::npos)
	{
		std::string str = fullFileName.c_str();
		return str;
	}
    else
        index += 1;

    return fullFileName.substr(index, fullFileName.length()-index);
}

#ifdef __LINUX__
void PathUtil::getFiles(std::vector<std::string>& vecFiles, const std::string& path, const std::string& suffix) 
{  
	DIR *dir;
	struct dirent *ptr;
	char base[1000];
	string mySuffix = suffix;
	StrUtil::removeBeginStr(mySuffix, "*");

	if ((dir=opendir(path.c_str())) == NULL)
	{
		//g_logger.log("Open dir error...");
		return;
	}

	while ((ptr=readdir(dir)) != NULL)
	{
		if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
		{
			continue;
		}
		else if(ptr->d_type == 8)    ///file
		{
			memset(base,'\0',sizeof(base));
			strcpy(base,path.c_str());
			strcat(base,ptr->d_name);
			string file_name = base;

			if (mySuffix == "")
			{
				vecFiles.push_back(file_name);	
			}
			else if(StrUtil::endWith(file_name, mySuffix))
			{
				vecFiles.push_back(file_name);	
			}
		}
		else if(ptr->d_type == 10)    ///link file
		{
		}
		else if(ptr->d_type == 4)    ///dir
		{
		}
	}
	closedir(dir);
} 
#else
void PathUtil::getFiles(std::vector<std::string>& vecFiles, const std::string& path, const std::string& suffix) 
{  
	long   hFile   =   0;  
	struct _finddata_t fileinfo;

	string findStr = path + suffix;
	if((hFile = _findfirst(findStr.c_str(),&fileinfo)) !=  -1)  
	{  
		do  
		{  
			if((fileinfo.attrib &  _A_SUBDIR))  
			{  
				//ignore
			}  
			else  
			{  
				vecFiles.push_back(path + fileinfo.name);  
			}  
		}while(_findnext(hFile, &fileinfo)  == 0);  
		_findclose(hFile);  
	}  
}
#endif
