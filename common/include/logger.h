#ifndef LOGGER_H__
#define LOGGER_H__

#include <string>
#include <mutex>

namespace comm
{
// 日志开关 是否追加 是否立刻刷新 多日志开关 最大字节数 是否向屏幕输出
#define DT_LOGPRINT			"DT_LOGPRINT"
#define DT_LOGAPPEND		"DT_LOGAPPEND"
#define DT_LOGFLUSH			"DT_LOGFLUSH"
#define DT_LOGMULTI			"DT_LOGMULTI"
#define DT_LOGMAXSIZE		"DT_LOGMAXSIZE"
#define DT_LOGSCREEN		"DT_LOGSCREEN"

class Logger
{
public:
	Logger(const std::string& fileName);
	Logger(const std::string& fileName, bool output, bool append);
	Logger(const std::string& fileName, bool output, bool append, bool flush, int max_size);
    ~Logger();
	void log(const std::string& str);
	void logFmt(const char* fmt, ...);
	FILE* getHandle();

    static bool GetLogPrint();
    static bool GetLogMulti();
    static bool GetLogAppend();

protected: 
	//成员变量不可随意增加，因LOGGER在mysock, mydns, myroute中作为参数传递（不好的设计，后续想办法去除偶合)
    std::string     m_fileName;
    FILE*           m_filePtr;
	bool            m_output;
    char*           m_szMsg;
	std::mutex      m_mutex;

private:
	void init(const std::string& fileName, bool output, bool append, bool flush, int max_size);
	Logger(const Logger& other) = delete;
	Logger& operator=(const Logger&) = delete;
};

std::string get_default_logpath();
void comm_printf(const char* fmt, ...);
Logger* get_null_logger();
}

#endif
