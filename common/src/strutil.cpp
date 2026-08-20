#include "strutil.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <iterator>
#include <cassert>
#include <iomanip>
#include <string.h>
#include <stdlib.h>
#include "md5sum.h"

#if defined(_WIN32) || defined(_WIN32_WCE)
#include <Windows.h>
#endif

#if 0//defined ANDROID
size_t mbstowcs(wchar_t *pwcs, const char *s, size_t n)
{
	if (pwcs == NULL)
		return strlen(s);
	return mbsrtowcs(pwcs, &s, n, NULL);
}

size_t wcstombs(char *s, const wchar_t *pwcs, size_t n)
{

	return wcsrtombs(s, &pwcs, n, NULL);
}
#endif

using namespace std;
using namespace comm;

void StrUtil::trim(std::string& src)
{
	if (src.empty())
	{
		return;
	}

	ltrim(src);
	rtrim(src);
}

void StrUtil::ltrim(std::string& src)
{
	if (src.empty())
	{
		return;
	}

	size_t index = src.find_first_not_of(" \t\r\n");
	if (index == string::npos) {
		src = "";
	}
	else {
		src = src.substr(index);
	}
}

void StrUtil::rtrim(std::string& src)
{
	if (src.empty())
	{
		return;
	}

	size_t index = src.find_last_not_of(" \t\r\n");
	if (index == string::npos) {
		src = "";
	}
	else {
		src = src.substr(0, index+1);
	}
}

void StrUtil::strcpy(char* dest, int full_size, const char* src)
{
	if (src == NULL)
	{
		return;
	}

	if (full_size <= 1)
	{
		printf("warning! can't be here (StrUtil_strcpy)");
		return;
	}

	snprintf(dest, full_size, "%s", src);
	dest[full_size-1] = '\0';
}

void StrUtil::strcat(char* dest, int full_size, const char* src)
{
	int dest_len = strlen(dest);
	int left_len = full_size - dest_len;

	if (left_len <= 0)
	{
		printf("warning! not enough space.");
		return;
	}

	if (src == NULL)
	{
		return;
	}

	if (full_size <= 1)
	{
		printf("warning! can't be here (StrUtil_strcat)");
		return;
	}

	snprintf(dest + dest_len, left_len, "%s", src);
}

#ifdef _WIN32
int strcasecmp(char *s1, char *s2)
{
	while  (toupper((unsigned char)*s1) == toupper((unsigned char)*s2++))
		if (*s1++ == '\0') return 0;
	return(toupper((unsigned char)*s1) - toupper((unsigned char)*--s2));
}

int strncasecmp(char *s1, char *s2, register int n)
{
	while (--n >= 0 && toupper((unsigned char)*s1) == toupper((unsigned char)*s2++))
		if (*s1++ == '\0')  return 0;
	return(n < 0 ? 0 : toupper((unsigned char)*s1) - toupper((unsigned char)*--s2));
}
#endif

char* StrUtil::strstri(const char* str, const char* subStr)
{
	int len = strlen(subStr);
	if(len == 0)
	{
		return NULL;
	}

	while(*str)
	{
		if(strncasecmp((char*)str, (char*)subStr, len) == 0)
		{
			return (char*)str;
		}
		++str;
	}
	return NULL;
}

bool StrUtil::hasContain(const std::string& src, const char* subStr)
{
	if (subStr==NULL || strcmp(subStr, "")==0)
	{
		return false;
	}

	return src.find(subStr) != string::npos;
}

int StrUtil::countOfStr(const std::string& src, const std::string& subStr)
{
	if (subStr.empty())
	{
		return 0;
	}

	int number = 0;
	int pos = 0;

	while(pos != string::npos)
	{
		pos = src.find(subStr, pos);
		if(pos != string::npos)
		{
			++number;
			pos += subStr.size();
		}
	}
	return number;
}

void StrUtil::replace(std::string &src, const char* oldStr, const char *newStr)
{
	if (src.empty())
	{
		return;
	}

	if (oldStr==NULL || strcmp(oldStr, "")==0 || newStr==NULL)
	{
		return;
	}

	int index = (int)src.find(oldStr);   
	while(index != string::npos)
	{   
		src.replace(index, strlen(oldStr), newStr);   
		index = (int)src.find(oldStr, index+strlen(newStr));   
	}
}

std::string StrUtil::combine(const StringArray& srcs, char spilt)
{
	if(srcs.empty())
	{
		return "";
	}

	string str_spilt = string(1, spilt);
	ostringstream oss;	
	ostream_iterator<string> os_iter(oss, str_spilt.c_str());
	copy(srcs.begin(), srcs.end(), os_iter);

	string str = oss.str();
	removeEndStr(str, str_spilt);

	return str;
}

std::string StrUtil::combine(const std::vector<int>& srcs, char spilt)
{
	if(srcs.empty())
	{
		return "";
	}

	string str_spilt = string(1, spilt);
	ostringstream oss;	
	ostream_iterator<int> os_iter(oss, str_spilt.c_str());
	copy(srcs.begin(), srcs.end(), os_iter);

	string str = oss.str();
	removeEndStr(str, str_spilt);

	return str;
}

void StrUtil::spilt(const std::string& src, char spilt, StringArray& strs, bool needTrim /*=true*/)
{
	if (src.empty()) {
		return; 
	}

	istringstream iss(src);
	while (!iss.eof())
	{
		string str;
		getline(iss, str, spilt);
		if (needTrim)
		{
			trim(str);
		}
		strs.push_back(str);
	}	

}

void StrUtil::spilt(const std::string& src, const char* spilt, StringArray& strs, bool needTrim /*=true*/)
{
	int pos = 0;
	int last_pos = 0;
	string elem;
	int spilt_len = strlen(spilt);

	while( (pos=src.find(spilt, last_pos)) != -1 )
	{
		elem = src.substr(last_pos, pos-last_pos);
		if (needTrim)
		{
			trim(elem);
		}
		strs.push_back(elem);
		last_pos = pos + spilt_len;
	}

	//last elem
	elem = src.substr(last_pos);
	if (needTrim)
	{
		trim(elem);
	}
	strs.push_back(elem);
}

void StrUtil::spilt2(const std::string& src, char spilt, comm::StringArray& strs, bool needTrim)
{
	if (src.empty()) {
		return; 
	}

	std::istringstream iss(src);
	while (!iss.eof())
	{
		std::string str;
		getline(iss, str, spilt);

		if (str.empty())
		{
			continue;
		}

		if (needTrim)
		{
			StrUtil::trim(str);
		}

		strs.push_back(str);				
	}
}


void StrUtil::spilt_keyvalues(const std::string& line, std::unordered_map<std::string, std::string>& mapKeyValue)
{
	spilt_keyvalues(line, mapKeyValue, "/r/n", "=");
}

void StrUtil::spilt_keyvalues(const std::string& line, std::unordered_map<std::string, std::string>& mapKeyValue, const std::string& line_split, const std::string& keyvalue_split)
{
	if (line=="" || line_split=="" || keyvalue_split=="")
	{
		return;
	}

	StringArray key_value_array;			
	StrUtil::spilt(line, line_split.c_str(), key_value_array);
	for (const auto& _key_value_line : key_value_array)
	{
		StringArray key_value;
		StrUtil::spilt(_key_value_line, keyvalue_split.c_str(), key_value);
		if (key_value.size() < 2)
		{
			continue;
		}

		string key = key_value[0];
		string value = key_value[1];
		mapKeyValue[key] = value;			
	}//for
}

std::string StrUtil::getSubBetween(const std::string& line, const std::string& beginStr, const std::string& endStr)
{
	if (line.empty())
	{
		return "";
	}

	if (beginStr.empty() && endStr.empty())
	{
		return "";
	}

	int begin_index = 0;
	if (!beginStr.empty())
	{
		begin_index = line.find(beginStr.c_str());
		if (begin_index == -1)
		{
			return "";
		}
	}	

	int start_pos = begin_index + beginStr.length();
	int end_index = line.length();
	if (!endStr.empty())
	{
		end_index = line.find(endStr, start_pos);
		if (end_index == -1)
		{
			return "";
		}
	}

	if (end_index <= start_pos)
	{
		return "";
	}
	
	return line.substr(start_pos, end_index - start_pos);
}

std::string StrUtil::grep(const std::string& content, const std::string& key)
{
	StringArray lines;
	StrUtil::spilt(content, "\n", lines);

	string out_lines;
	for (const auto& _line : lines)
	{
		string line = _line;
		if (StrUtil::hasContain(line, key.c_str()))
		{
			out_lines += line;
			out_lines += "\n";
		}
	}
	StrUtil::removeEndStr(out_lines, "\n");
	return out_lines;
}

std::string StrUtil::awk(const std::string& content, int field, char spilt)
{
	if (field <= 0)
	{
		return "";
	}

	StringArray items;
	StrUtil::spilt2(content, spilt, items);
	if ((int)items.size() >= field)
	{
		return items[field-1];
	}
	return "";
}

bool StrUtil::beginWith(const std::string& src, const std::string& beginStr)
{
	if (src.empty() || beginStr.empty())
	{
		return false;
	}

	if (src.length() <beginStr.length())
	{
		return false;
	}

	if (src.substr(0, beginStr.length()) == beginStr)
	{
		return true;
	}

	return false;
}

bool StrUtil::endWith(const std::string& src, const std::string& endStr)
{
	if (src.empty() || endStr.empty())
	{
		return false;
	}

	if (src.length() < endStr.length())
	{
		return false;
	}
	
	if (src.substr(src.length() - endStr.length())==endStr)
	{
		return true;
	}
	
	return false;
}

void  StrUtil::removeBeginStr(std::string& src, const std::string& begStr)
{
	if (beginWith(src, begStr))
	{
		src = src.substr(begStr.length());
	}
}

void StrUtil::removeEndStr(std::string& src, const std::string& endStr)
{
	if (endWith(src, endStr))
	{
		src = src.erase(src.length() - endStr.length());
	}
}

std::string StrUtil::Int2Str(int val)
{
	char temp[100];
	sprintf(temp, "%d", val);
	return temp;
}

std::string StrUtil::UInt2Str(unsigned int val)
{
	char temp[100];
	sprintf(temp, "%u", val);
	return temp;
}

std::string StrUtil::Int642Str(INT64 val)
{
	char temp[100];
	sprintf(temp, INT64OUTFORMAT, val);
	return temp;
}

std::string StrUtil::UInt642Str(UINT64 val)
{
	char temp[100];
	sprintf(temp, UINT64OUTFORMAT, val);
	return temp;
}

std::string StrUtil::Dbl2Str(double val)
{
	char temp[100];
	sprintf(temp, "%.2f", val);
	return temp;
}

std::string StrUtil::Dbl2StrHigh(double val)
{
	char temp[100];
	sprintf(temp, "%.7f", val);
	return temp;
}

int StrUtil::Str2Int(const std::string& val)
{
	return atoi(val.c_str());
}

unsigned int StrUtil::Str2UInt(const std::string& val)
{
	unsigned int value = 0;
	sscanf(val.c_str(), "%u", &value);
	return value;
}

INT64 StrUtil::Str2Int64(const std::string& val)
{
#if defined(WIN32) || defined(_WIN32_WCE)
	return _atoi64(val.c_str());
#else
	return atoll(val.c_str());
#endif
}

double StrUtil::Str2Dbl(const std::string& val)
{
	return atof(val.c_str());
}

bool StrUtil::isNumeric(const std::string& val)
{
	if (val.empty())
	{
		return false;
	}

	return std::all_of(val.begin(), val.end(), [](char _ch)
	{
		return std::isdigit(static_cast<unsigned char>(_ch)) != 0;
	});
}

bool StrUtil::isDbl(const std::string& val)
{
	if (val.empty())
	{
		return false;
	}

	return std::all_of(val.begin(), val.end(), [](char _ch)
	{
		return _ch == '.' || std::isdigit(static_cast<unsigned char>(_ch)) != 0;
	});
}

int StrUtil::atoi_radix(char * str)
{
	int value = 0;
	int sign = 1;
	int radix;

	if(*str == '-')
	{
		sign = -1;
		str++;
	}
	if(*str == '0' && (*(str+1) == 'x' || *(str+1) == 'X'))
	{
		radix = 16;
		str += 2;
	}
	else if(*str == '0') //0开头是八进制
	{
		radix = 8;
		str++;
	}
	else
		radix = 10;

	while(*str)
	{
		if(radix == 16)
		{
			if(*str >= '0' && *str <= '9')
				value = value * radix + *str - '0';    //0到9的字符串
			else if(*str >= 'A' && *str <= 'F')
				value = value * radix + *str - 'A' + 10;   //大写十六进制的ABCDEF的字符串的情况
			else if(*str >= 'a' && *str <= 'f')
				value = value * radix + *str - 'a' + 10;  // 小写十六进制的abcdef的字符串的情况
		}
		else
			value = value * radix + *str - '0';
		str++;
	}
	return sign*value;
}


void StrUtil::toUpper(std::string& src)
{
	std::transform(src.begin(), src.end(), src.begin(), [](char _ch)
	{
		return static_cast<char>(std::toupper(static_cast<unsigned char>(_ch)));
	});
}

void StrUtil::toLower(std::string& src)
{
	std::transform(src.begin(), src.end(), src.begin(), [](char _ch)
	{
		return static_cast<char>(std::tolower(static_cast<unsigned char>(_ch)));
	});
}

#if defined(WIN32) || defined(_WIN32_WCE)
std::string StrUtil::Unicode2Mbcs(const std::wstring& inWStr, int code_page)
{
	int length = WideCharToMultiByte (code_page, NULL, inWStr.c_str(), -1, NULL, 0, NULL, FALSE);
	char* pStr = new char[length + 1];
	ZeroMemory(pStr, sizeof(char) * (length+1));

	WideCharToMultiByte (code_page, NULL, inWStr.c_str(), -1, pStr, 
			length, NULL, FALSE);

	string str = pStr;
	delete[] pStr;
	return str;
}

std::wstring StrUtil::Mbcs2Unicode(const std::string& inStr, int code_page)
{
	int length = MultiByteToWideChar(code_page, NULL, inStr.c_str(), -1,  NULL, 0);
	wchar_t* pWideStr = new wchar_t[length + 1];
	ZeroMemory(pWideStr, sizeof(wchar_t) * (length + 1));
	
	MultiByteToWideChar(code_page, NULL, inStr.c_str(), -1, pWideStr, length);
	
	wstring wideStr = pWideStr;
	delete[] pWideStr;
	return wideStr;
}

std::string StrUtil::Local2Utf8(const std::string& inStr)
{
	std::wstring str_unicode = StrUtil::Mbcs2Unicode(inStr);	
	return StrUtil::Unicode2Mbcs(str_unicode, CP_UTF8);
}

std::string StrUtil::Utf82Local(const std::string& inStr)
{
	string inStr2 = inStr;
	StrUtil::replace(inStr2, "\xC2\xA0", ""); //UTF8排版空格C2A0
	std::wstring str_unicode = StrUtil::Mbcs2Unicode(inStr2, CP_UTF8);	
	return StrUtil::Unicode2Mbcs(str_unicode, CP_OEMCP);
}
#endif

std::string StrUtil::Local2Utf8IfNeed(const std::string& inStr)
{
#if defined(WIN32) || defined(_WIN32_WCE)
	return Local2Utf8(inStr);
#else
	return inStr;
#endif	
}

void StrUtil::prepareOSS(std::ostringstream& oss)
{
	//prevent 1,200 format
#ifdef _WIN32
	oss.imbue(std::locale(oss.getloc()));
#endif
}

string StrUtil::getUri(std::unordered_map<string,string>& paramsMap)
{
	string result_str;
	for (const auto& _param : paramsMap)
	{
		result_str += result_str.empty() ? "?" : "&";
		result_str += _param.first + "=" + _param.second;
	}
	return result_str;
}

string StrUtil::getStrMd5(const std::string& str)
{
	char buff_md5[128];
	memset(buff_md5, 0x00, sizeof(buff_md5));
	if(md5sum_str(str.c_str(), str.length(), buff_md5) == 0)
	{
		return string(buff_md5);
	}
	return "";
}
