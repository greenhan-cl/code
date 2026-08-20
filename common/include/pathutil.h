#ifndef PATH_UTIL_H__
#define PATH_UTIL_H__

#include <string>
#include <vector>

#define DT_EXEPATH "DT_EXEPATH"
#define DT_LOGPATH "DT_LOGPATH"
#define DT_INIPATH "DT_INIPATH"

#if defined(WIN32) || defined(_WIN32_WCE)
#define LOG_PATH "log\\"
#define PATH_SEPA "\\"
#else
#define LOG_PATH "log/"
#define PATH_SEPA "/"
#endif

namespace comm
{
/**
* 路径相关函数  
*/
class PathUtil
{
public:
	static std::string GetExePathA();
	static std::string GetIniPath();
    static std::string GetLogPath();
	static std::wstring GetExePathW();

	static bool	PathExist(const std::string& path);
	static bool PathCreate(const std::string& path);

    static std::string getPathOfFullFilename(const std::string& fullFileName);
    static std::string getFilenameOfFullFilename(const std::string& fullFileName);
	//get files in path(not contian subdir), suffix must as: *.zip or *
	static void getFiles(std::vector<std::string>& vecFiles, const std::string& path, const std::string& suffix) ;
};

}

#endif
