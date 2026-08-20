#include "inifile.h"
#include "exputil.h"
#include "strutil.h"
#include "fileutil.h"
#include <string.h>

// MSVC versions before VS2015 do not provide the standard snprintf.
// Newer Universal CRT headers reject redefining snprintf as a macro.
#if (defined(__WIN32__) || (defined(_WIN32) && !defined(__SYMBIAN32__))) \
    && defined(_MSC_VER) && _MSC_VER < 1900
#define snprintf _snprintf
#endif

#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;
using namespace comm;

static const int LINE_LENGTH = 10240;

//algorithm for find_if
class record_equal
{
public:
	record_equal(const std::string& section, const std::string& key)
		: m_section(section), m_key(key) {}
	bool operator() (const Record& rec){
		return rec.section == m_section && rec.key == m_key;
	}
private:
	std::string m_section;
	std::string m_key;
};

CIniFile::CIniFile()
{
	m_bModified = false;
}

bool CIniFile::load(const std::string& fileName, bool createFlag)
{
	m_fileName = fileName;
	m_createFlag = createFlag;

	if (!LoadFile())
	{
		if (createFlag)
		{
			//file not exist, create it
			if(!Save())
			{
				//create failed, throw exception
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	m_bModified = false;
	return true;
}

CIniFile::~CIniFile()
{
	if (m_bModified)
	{
		Save();
	}
}

void CIniFile::GetValue(const char* section, const char* key, const char* defaultValue, 
							   char* outbuf, unsigned int nSize)
{
	RecordList::iterator iter = find_if(m_records.begin(), m_records.end(), record_equal(section, key));

	if (iter != m_records.end())
	{
		snprintf(outbuf, nSize-1, "%s", iter->value.c_str());
	}
	else
	{
		snprintf(outbuf, nSize-1, "%s", defaultValue);
	}
}

int CIniFile::GetValueInt(const char* section, const char* key, int defaultValue)
{
	char defBuff[LINE_LENGTH];
	snprintf(defBuff, LINE_LENGTH-1, "%d", defaultValue);

	char retBuff[LINE_LENGTH];
	GetValue(section, key, defBuff, retBuff, sizeof(retBuff));
	return atoi(retBuff);
}

double CIniFile::GetValueReal(const char* section, const char* key, double defaultValue)
{
	char defBuff[LINE_LENGTH];
	snprintf(defBuff, LINE_LENGTH-1, "%lf", defaultValue);
		
	char retBuff[LINE_LENGTH];
	GetValue(section, key, defBuff, retBuff, sizeof(retBuff));
	return atof(retBuff);
}

void CIniFile::SetValue(const char* section, const char* key, const char* value)
{
	m_bModified = true;

	list<Record>::iterator iter = m_records.begin();
	list<Record>::iterator section_iter = m_records.end();
	
	//find section iter
	for (; iter!=m_records.end(); iter++)
	{
		if (iter->section == section)
		{
			section_iter = iter;
			break;
		}
	}

	//not found section
	if (section_iter == m_records.end()) 
	{
		//append section 
		Record rec_section;
		rec_section.section = section;
		rec_section.key = "";
		rec_section.value = "";
		rec_section.comment = "";
		m_records.push_back(rec_section);
		
		//append key 
		Record rec_key;
		rec_key.section = section;
		rec_key.key = key;
		rec_key.value = value;
		rec_key.comment = "";
		m_records.push_back(rec_key);

		return;
	}

	//find key iter
	iter++;
	for (; iter!=m_records.end(); iter++)
	{
		if (iter->section != section)
		{
			//not found
			break;
		}

		if (iter->key == key)
		{
			//found, set value
			iter->value = value;
			return;
		}
	}

	//not found key, append
	Record rec;
	rec.section = section;
	rec.key = key;
	rec.value = value;
	rec.comment = "";			
	m_records.insert(iter, rec);
}

void CIniFile::SetValueInt(const char* section, const char* key, int value)
{
	m_bModified = true;

	char buff[LINE_LENGTH];
	snprintf(buff, LINE_LENGTH-1, "%d", value);

	SetValue(section, key, buff);
}

void CIniFile::SetValueReal(const char* section, const char* key, double value)
{
	m_bModified = true;

	char buff[LINE_LENGTH];
	snprintf(buff, LINE_LENGTH-1, "%lf", value);
	
	SetValue(section, key, buff);
}

void CIniFile::SetSectionComment(const char* section, const char* comment)
{
	RecordList::iterator iter = find_if(m_records.begin(), m_records.end(), record_equal(section, ""));

	if (iter != m_records.end())
	{
		m_bModified = true;

		if (strcmp(comment, "") == 0)
		{
			iter->comment = "";	//clear comment
		}
		else
		{
			iter->comment = string("#") + comment + "\n";
		}
	}
}

void CIniFile::SetKeyComment(const char* section,  const char* key, const char* comment)
{
	RecordList::iterator iter = find_if(m_records.begin(), m_records.end(), record_equal(section, key));
	if (iter != m_records.end())
	{
		if (strcmp(comment, "") == 0)
		{
			iter->comment = "";	//clear comment
		}
		else
		{
			iter->comment = string("#") + comment + "\n";
		}

		m_bModified = true;
	}
}

void CIniFile::DeleteSection(const char* section)
{
	//find section iter
	RecordList::iterator del_begin_iter = find_if(m_records.begin(), m_records.end(), record_equal(section, ""));
	if (del_begin_iter == m_records.end()) 
	{
		return;
	}
	
	//find section's key iter
	RecordList::iterator del_end_iter = del_begin_iter;
	for (; del_end_iter!=m_records.end(); del_end_iter++)
	{
		if (del_end_iter->section != section)
		{
			break;
		}		
	}

	//delete section & section's keys
	m_records.erase(del_begin_iter, del_end_iter);
	m_bModified = true;
}

//delete section's keys
void CIniFile::DeleteKeys(const char* section)
{
	//find section iter
	RecordList::iterator section_iter = find_if(m_records.begin(), m_records.end(), record_equal(section, ""));
	if (section_iter == m_records.end()) 
	{
		return;
	}
	
	RecordList::iterator del_begin_iter = section_iter;
	del_begin_iter++; //not delete section
	
	//find section's key iter
	RecordList::iterator del_end_iter = del_begin_iter;
	for (; del_end_iter!=m_records.end(); del_end_iter++)
	{
		if (del_end_iter->section != section)
		{
			break;
		}		
	}
	
	//delete section's keys
	m_records.erase(del_begin_iter, del_end_iter);
	m_bModified = true;
}

void CIniFile::DeleteKey(const char* section, const char* key)
{
	RecordList::iterator del_iter = find_if(m_records.begin(), m_records.end(), record_equal(section, key));
	if (del_iter != m_records.end())
	{
		m_records.erase(del_iter);
		m_bModified = true;
	}
}

bool CIniFile::LoadFile()
{
	ifstream inFile;
	if (!FileUtil::open_ifstream(inFile, m_fileName))
	{
		return false; //file not exist
	}
	
	char szLineBuff[LINE_LENGTH];
	string line = "";
	string current_section = "";
	string comment = "";
	size_t idx = string::npos;
	
	while (inFile.getline(szLineBuff, LINE_LENGTH))
	{
		line = szLineBuff;
		StrUtil::trim(line);
		if (line == "")	//ignore blank line 
		{
			continue;
		}
		
		if(line[0]=='#' || line[0]==';') //comment
		{
			comment += line + '\n';
		}
		else if(line.length()>2 && line[0]=='[' && line[line.length()-1]==']') //section
		{
			Record rec;
			rec.section = line.substr(1, line.length() - 2);
			rec.key = "";
			rec.value = "";
			rec.comment = comment;	
			
			m_records.push_back(rec);
			comment = "";
			current_section = rec.section;
		}
		else if( (idx=line.find('='))!=line.npos && idx>0)	//Key
		{
			Record rec;
			rec.section = current_section;
			rec.key  = line.substr(0, idx);
			StrUtil::rtrim(rec.key);			
			if ((idx + 1) == line.length())
			{
				rec.value = "";  //line as: key=
			}
			else
			{
				rec.value = line.substr(idx + 1);					
				StrUtil::ltrim(rec.value);
			}
			rec.comment = comment;
			
			m_records.push_back(rec);			
			comment = "";
		}
		else
		{
			//invalid line. ignore
		}
	}

	inFile.close();
	return true;
}

bool CIniFile::Save()
{
	if (!m_bModified)
	{
		return true;
	}

	ofstream outFile;
	if (!FileUtil::open_ofstream(outFile, m_fileName))
	{
		return false; //file not exist
	}
	
	bool firstSection = true;
	list<Record>::iterator iter=m_records.begin();

	for (; iter!=m_records.end(); iter++)
	{
		if (iter->key == "")	//section
		{
			if (firstSection)
			{
				firstSection = false;
			}
			else
			{
				outFile << endl;  //blank line to split sections
			}
			outFile << iter->comment;
			outFile << "[" <<iter->section << "]" <<endl;
		}
		else			//key
		{
			outFile << iter->comment;
			outFile << iter->key << "=" <<iter->value <<endl;
		}
	}

	if (outFile.bad())
	{
		outFile.close();
		m_bModified = false; 
		return false;
	}
	
	outFile.close();
	m_bModified = false; 
	return true;
}

