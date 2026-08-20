#ifndef CINIFILE_H__
#define CINIFILE_H__

#include <string>
#include <list>
#include <map>

namespace comm
{

struct Record
{
	std::string section;
	std::string key;
	std::string value;
	std::string comment;
};

class CIniFile
{
public:
	CIniFile();
	~CIniFile();

	bool load(const std::string& fileName, bool createFlag=false);
	bool Save();

	void GetValue(const char* section, const char* key, const char* defaultValue, char* outbuf, unsigned int nSize);
	int GetValueInt(const char* section, const char* key, int defaultValue);
	double GetValueReal(const char* section, const char* key, double defaultValue);

	void SetValue(const char* section, const char* key, const char* value);
	void SetValueInt(const char* section, const char* key, int value);
	void SetValueReal(const char* section, const char* key, double value);

	 //if comment is not blank, then format comment as: "#comment\n"
	void SetSectionComment(const char* section, const char* comment);
	void SetKeyComment(const char* section,  const char* key, const char* comment);

	//delete section & section's keys
	void DeleteSection(const char* section);
	//delete section's keys
	void DeleteKeys(const char* section);
	void DeleteKey(const char* section, const char* key);
	
private:
	bool LoadFile();

private:	
	std::string			m_fileName;
	bool				m_createFlag;
	typedef std::list<Record> RecordList;
	RecordList			m_records;	
	bool				m_bModified;
};

} //NAMESPACE

#endif	/* CINIFILE_H__*/

