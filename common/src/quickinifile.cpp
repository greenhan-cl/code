#include "quickinifile.h"
#include "exputil.h"
#include "strutil.h"
#include "fileutil.h"
#include <string.h>

// MSVC versions before VS2015 do not provide the standard snprintf.
// Newer Universal CRT headers reject redefining snprintf as a macro.
#if defined(_WIN32) && defined(_MSC_VER) && _MSC_VER < 1900
#define snprintf _snprintf
#endif

#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;
using namespace comm;

static const int LINE_LENGTH = 10240;

CQuickIniFile::CQuickIniFile()
{
	m_bModified = false;
}

bool CQuickIniFile::load(const std::string& fileName, bool createFlag)
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

CQuickIniFile::~CQuickIniFile()
{
	if (m_bModified)
	{
		Save();
	}
}

string CQuickIniFile::getSecKey(const char* section, const char* key)
{
	string sec_key = string(section)  + "|" + key;
	return sec_key;
}

void CQuickIniFile::spiltSecKey(const std::string& sec_key, std::string& section, std::string& key)
{
	StringArray elems;
	StrUtil::spilt(sec_key, "|", elems);
	if (elems.size() >= 2)
	{
		section = elems[0];
		key = elems[1];
	}
}

void CQuickIniFile::GetValue(const char* section, const char* key, const char* defaultValue, 
						char* outbuf, unsigned int nSize)
{
	string sec_key = getSecKey(section, key);
	map<string, string>::iterator iter = m_records.find(sec_key);

	if (iter != m_records.end())
	{
		snprintf(outbuf, nSize-1, "%s", iter->second.c_str());
	}
	else
	{
		snprintf(outbuf, nSize-1, "%s", defaultValue);
	}
}

int CQuickIniFile::GetValueInt(const char* section, const char* key, int defaultValue)
{
	char defBuff[LINE_LENGTH];
	snprintf(defBuff, LINE_LENGTH-1, "%d", defaultValue);

	char retBuff[LINE_LENGTH];
	GetValue(section, key, defBuff, retBuff, sizeof(retBuff));
	return atoi(retBuff);
}

double CQuickIniFile::GetValueReal(const char* section, const char* key, double defaultValue)
{
	char defBuff[LINE_LENGTH];
	snprintf(defBuff, LINE_LENGTH-1, "%lf", defaultValue);

	char retBuff[LINE_LENGTH];
	GetValue(section, key, defBuff, retBuff, sizeof(retBuff));
	return atof(retBuff);
}

void CQuickIniFile::SetValue(const char* section, const char* key, const char* value)
{
	m_bModified = true;
	string sec_key = getSecKey(section, key);
	m_records[sec_key] = string(value);
}

void CQuickIniFile::SetValueInt(const char* section, const char* key, int value)
{
	m_bModified = true;

	char buff[LINE_LENGTH];
	snprintf(buff, LINE_LENGTH-1, "%d", value);

	SetValue(section, key, buff);
}

void CQuickIniFile::SetValueReal(const char* section, const char* key, double value)
{
	m_bModified = true;

	char buff[LINE_LENGTH];
	snprintf(buff, LINE_LENGTH-1, "%lf", value);

	SetValue(section, key, buff);
}

bool CQuickIniFile::LoadFile()
{
	ifstream inFile;
	if (!FileUtil::open_ifstream(inFile, m_fileName))
	{
		return false; //file not exist
	}

	char szLineBuff[LINE_LENGTH];
	string line = "";
	string current_section = "";
	size_t idx = string::npos;

	while (inFile.getline(szLineBuff, LINE_LENGTH))
	{
		line = szLineBuff;
		StrUtil::trim(line);
		if (line == "")	//ignore blank line 
		{
			continue;
		}
		else if(line[0]=='#' || line[0]==';') //comment
		{
			continue;
		}
		else if(line.length()>2 && line[0]=='[' && line[line.length()-1]==']') //section
		{
			current_section = line.substr(1, line.length() - 2);
		}
		else if( (idx=line.find('='))!=line.npos && idx>0)	//Key
		{
			string section = current_section;
			string key  = line.substr(0, idx);
			StrUtil::trim(key);			

			string value = "";
			if ((idx + 1) != line.length())
			{
				value = line.substr(idx + 1);					
				StrUtil::trim(value);
			}

			string sec_key = getSecKey(section.c_str(), key.c_str());
			m_records[sec_key] = string(value);
		}
		else
		{
			//invalid line. ignore
		}
	}

	inFile.close();
	return true;
}

bool CQuickIniFile::Save()
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

	string last_section = "";
	map<string, string>::iterator iter=m_records.begin();

	for (; iter!=m_records.end(); iter++)
	{
		string section = "";
		string key = "";
		spiltSecKey(iter->first, section, key);

		if (section!="" && key!="")
		{
			if (last_section != section)
			{
				if (last_section!="")
				{
					outFile << endl;
				}
				outFile << "[" <<section << "]" <<endl;
				last_section = section;
			}

			outFile << key << "=" << iter->second <<endl;
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

