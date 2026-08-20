#ifndef QUICK_INI_FILE
#define  QUICK_INI_FILE

#include <string>
#include <map>

namespace comm
{
//should use in big ini file
// should use in only read. save will change order
//line_maxsize 10K
class CQuickIniFile
{
public:
	CQuickIniFile();
	~CQuickIniFile();

	bool load(const std::string& fileName, bool createFlag=false);
	bool Save();

	void GetValue(const char* section, const char* key, const char* defaultValue, char* outbuf, unsigned int nSize);
	int GetValueInt(const char* section, const char* key, int defaultValue);
	double GetValueReal(const char* section, const char* key, double defaultValue);

	void SetValue(const char* section, const char* key, const char* value);
	void SetValueInt(const char* section, const char* key, int value);
	void SetValueReal(const char* section, const char* key, double value);

private:
	bool LoadFile();
	std::string getSecKey(const char* section, const char* key);
	void spiltSecKey(const std::string& sec_key, std::string& section, std::string& key);

private:	
	std::string		m_fileName;
	bool				m_createFlag;
	std::map<std::string, std::string> m_records;	
	bool				m_bModified;
};

}
#endif
