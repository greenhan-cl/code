#ifndef STRUTIL_H__
#define STRUTIL_H__

#include <string>
#include <vector>
#include <unordered_map>
#include <ostream>
#include "platinc.h"

#ifdef _WIN32
int strcasecmp(char *s1, char *s2);
int strncasecmp(char *s1, char *s2, register int n);
#endif

namespace comm
{
#if defined(__LINUX__) || defined ANDROID
	#define CP_OEMCP	1  
#endif

typedef std::vector<std::string> StringArray;

class StrUtil
{
public:
	//去除开头或结果空格
	static void trim(std::string& src);
	//去除左侧空格
	static void ltrim(std::string& src);
	//去除右侧空格
	static void rtrim(std::string& src);
	//字符串拷贝，处理了边界问题
	static void strcpy(char* dest, int full_size, const char* src);
	//追加写入，处理了边界问题
	static void strcat(char* dest, int full_size, const char* src);

	//查找替换
	//不区分大小写查找
	static char* strstri(const char* str, const char* subStr);
	//judge字串
	static bool hasContain(const std::string& src, const char* subStr);
	//统计出现次数
	static int countOfStr(const std::string& src, const std::string& subStr);
	//替换匹配的字串
	static void replace(std::string &src, const char* oldStr, const char *newStr);

	
	//一个字符连接多个字符串
	static std::string combine(const StringArray& srcs, char spilt);
	//一个字符连接多个整数
	static std::string combine(const std::vector<int>& srcs, char spilt);
	//按字符拆分
	static void spilt(const std::string& src, char spilt, StringArray& strs, bool needTrim=true);
	static void spilt(const std::string& src, const char* spilt, StringArray& strs, bool needTrim=true);
	//与spilt区别是:多分割符连一起算1个分割符
	static void spilt2(const std::string& src, char spilt, comm::StringArray& strs, bool needTrim =true);
	//分割形如：key1=value1/r/nkey2=value2/r/n
	static void spilt_keyvalues(const std::string& line, std::unordered_map<std::string, std::string>& mapKeyValue);
	static void spilt_keyvalues(const std::string& line, std::unordered_map<std::string, std::string>& mapKeyValue, const std::string& line_split, const std::string& keyvalue_split);
	//截取开始和结束直接的内容
	static std::string getSubBetween(const std::string& line, const std::string& beginStr, const std::string& endStr);

	static std::string grep(const std::string& content, const std::string& key);
	//field base from 1
	static std::string awk(const std::string& content, int field, char spilt=' ');

	//判断开头是否为指定的字符串
	static bool beginWith(const std::string& src, const std::string& beginStr);
	//判断结尾是否为指定的字符串
	static bool endWith(const std::string& src, const std::string& endStr);
	//判断开头是否为指定的字符串，删除
	static void removeBeginStr(std::string& src, const std::string& begStr);
	//判断结尾是否为指定的字符串，删除
	static void removeEndStr(std::string& src, const std::string& endStr);

	//转换
	static std::string Int2Str(int val);
	static std::string UInt2Str(unsigned int val);
	static std::string Int642Str(INT64 val);
	static std::string UInt642Str(UINT64 val);
	static std::string Dbl2Str(double val);
	static std::string Dbl2StrHigh(double val);
	static int Str2Int(const std::string& val);
	static unsigned int Str2UInt(const std::string& val);
	static INT64 Str2Int64(const std::string& val);
	static double Str2Dbl(const std::string& val);
	static int atoi_radix(char * str);

#if defined(_WIN32) || defined(_WIN32_WCE)
	static std::string Unicode2Mbcs(const std::wstring& inWStr, int code_page=CP_OEMCP);
	static std::wstring Mbcs2Unicode(const std::string& inStr, int code_page=CP_OEMCP);
	static std::string Local2Utf8(const std::string& inStr);
	static std::string Utf82Local(const std::string& inStr);
#endif

	//only for win32's compile enviroment
	static std::string Local2Utf8IfNeed(const std::string& inStr);
	
	//判断是否全是数值
	static bool isNumeric(const std::string& val);	
	static bool isDbl(const std::string& val);	

    //转换大小写
    static void toUpper(std::string& src);
    static void toLower(std::string& src);
	static void prepareOSS(std::ostringstream& oss);
	//return format : &key1=value1&key2=value2...
	static std::string getUri(std::unordered_map<std::string, std::string>& paramsMap);
	static std::string getStrMd5(const std::string& str);

private:
	StrUtil();
};

} //namespace

#endif	//STRUTIL_H__
