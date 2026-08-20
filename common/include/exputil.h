#ifndef EXPUTIL_H__
#define EXPUTIL_H__

#include <exception>
#include <string>

namespace comm
{
#define ERRCODE_FILE_NOTEXIST			9001
#define ERRCODE_FILE_OPENFAILED			9002
#define ERRCODE_FILE_CREATEFAILED		9003
#define ERRCODE_FAILED_LOAD_LIBRARY		9004
#define ERRCODE_UNKNOWN					9005

/**
* 自定义异常
*/
class MyException : public std::exception
{
public:
	MyException(int err_code, const std::string& err_descr, int err_code2=0)
		: m_errcode(err_code),
		  m_errcode2(err_code2),
		  m_what(err_descr)
	{
	}

	virtual const char* what() const noexcept override
	{
		return m_what.c_str();
	}

	int GetErrCode() const
	{
		return m_errcode;
	}

	int GetErrCode2() const 
	{
		return m_errcode2;
	}

protected:
	int m_errcode;
	int m_errcode2; //小分类代码

	std::string m_what;
};

}

#endif //EXPUTIL_H__
