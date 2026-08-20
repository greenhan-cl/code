#include "fileutil.h"
#include "exputil.h"

#include <string.h>
#include <fcntl.h>
#include<sys/stat.h>

#if defined(WIN32)
#include <io.h>
#else
#include <dirent.h> 
#endif

#include <sstream>
#include "strutil.h"
#include "timeutil.h"
#include "md5sum.h"

using namespace std;
using namespace comm;

bool FileUtil::open_ifstream(std::ifstream& stm, const std::string& fileName)
{
#if defined(_WIN32) && !defined(_WIN32_WCE)
	locale loc = locale::global(locale(""));
#endif	
	stm.open(fileName.c_str());
#if defined(_WIN32) && !defined(_WIN32_WCE)
	locale::global(loc);
#endif

	return stm.is_open();
}

bool FileUtil::open_ifstream(std::ifstream& stm, const std::string& fileName,
#if defined(_WIN32) || defined(_WIN32_WCE)
std::ios_base::open_mode mode)
#elif defined(__IPHONEOS__) || defined(ANDROID)
std::ios_base::openmode mode)
#else
std::_Ios_Openmode mode)
#endif
{
#if defined(_WIN32) && !defined(_WIN32_WCE)
	locale loc = locale::global(locale(""));
#endif
	stm.open(fileName.c_str(), mode);
#if defined(_WIN32) && !defined(_WIN32_WCE)
	locale::global(loc);
#endif
	return stm.is_open();
}

bool FileUtil::open_ofstream(std::ofstream& stm, const std::string& fileName)
{
#if defined(_WIN32) && !defined(_WIN32_WCE)
	locale loc = locale::global(locale(""));
#endif
	stm.open(fileName.c_str());
#if defined(_WIN32) && !defined(_WIN32_WCE)
	locale::global(loc);
#endif
	return stm.is_open();
}

bool FileUtil::open_ofstream(std::ofstream& stm, const std::string& fileName,
#if defined(_WIN32) || defined(_WIN32_WCE) || defined(__SYMBIAN32__)
std::ios_base::open_mode mode)
#elif defined(__IPHONEOS__) || defined(ANDROID)
std::ios_base::openmode mode)
#else
std::_Ios_Openmode mode)
#endif
{
#if defined(_WIN32) && !defined(_WIN32_WCE)
	locale loc = locale::global(locale(""));
#endif
	stm.open(fileName.c_str(), mode);
#if defined(_WIN32) && !defined(_WIN32_WCE)
	locale::global(loc);
#endif
	return stm.is_open();
}

bool FileUtil::file_exist(const std::string& fileName)
{
	ifstream stm;
#if defined(_WIN32) && !defined(_WIN32_WCE)
	locale loc = locale::global(locale(""));
#endif	
	stm.open(fileName.c_str());
#if defined(_WIN32) && !defined(_WIN32_WCE)
	locale::global(loc);
#endif
	bool exist = stm.is_open();
	stm.close();
	return exist;
}

std::string FileUtil::getBaseName(const std::string& filename)
{
    int index = filename.find_last_of(".");
    if (index == string::npos)
        return filename;
    
    return filename.substr(0, index);
}

std::string FileUtil::getExtName(const std::string& filename)
{
    int index = filename.find_last_of(".");
    if (index == string::npos)
        return "";
    else
        index += 1;
    
    return filename.substr(index, filename.length()-index);
}

int FileUtil::getFileSize(const std::string& filename)
{
	struct stat st;
	memset (&st, 0, sizeof(struct stat));
	if (stat (filename.c_str(), &st) == 0 )
		return st.st_size;
	return -1;
}

int FileUtil::getFileSize(FILE * file_handle)
{
	struct stat st;
	memset (&st, 0, sizeof(struct stat));
#ifdef  __LINUX__
	if (fstat (fileno(file_handle), &st) == 0 )
#else
	if (fstat (_fileno(file_handle), &st) == 0 )
#endif
		return st.st_size;
	return -1;
}

string FileUtil::getFileCreateTime(const std::string& filename)
{
	struct stat st;
	memset (&st, 0, sizeof(struct stat));
	if (stat (filename.c_str(), &st) == 0 )
		return TimeUtil::format_timet("%04d-%02d-%02d %02d:%02d:%02d",  st.st_ctime);
	return "";
}

string FileUtil::getFileModifyTime(const std::string& filename)
{
	struct stat st;
	memset (&st, 0, sizeof(struct stat));
	if (stat (filename.c_str(), &st) == 0 )
		return TimeUtil::format_timet("%04d-%02d-%02d %02d:%02d:%02d",  st.st_mtime);
	return "";
}

string FileUtil::getFileMd5(const std::string& filename)
{
	char buff_md5[128];
	memset(buff_md5, 0x00, sizeof(buff_md5));
	if(md5sum_file(filename.c_str(), buff_md5) == 0)
	{
		return string(buff_md5);
	}
	return "";
}

void FileUtil::deleteFile(const std::string& filename)
{
	remove(filename.c_str());
}

bool FileUtil::file_copy(const string& src, const string& dst)
{
	std::ifstream fin;
	if(!open_ifstream(fin, src, std::ios::binary)){
		return false;
	}

	std::ofstream out;
	if(!open_ofstream(out, dst, std::ios::binary)){
		return false;
	}    
	out << fin.rdbuf();
	return true;
}

string FileUtil::file_read(const string& file){
	std::ifstream fin;
	if(!open_ifstream(fin, file, std::ios::binary)){
		return "";
	}

	std::stringstream oss;
	oss << fin.rdbuf();
	return oss.str();
}

long FileUtil::file_read(const string& file, unsigned char* data, int len)
{
	std::ifstream fin;
	if(!open_ifstream(fin, file, std::ios::binary)){
		return 0;
	}

	fin.read((char*)data, len);
	return fin.gcount();
}

bool FileUtil::file_read_lines(const std::string& file, std::vector<std::string>& vecLines)
{
	string content = FileUtil::file_read(file);
	if (content.empty())
	{
		return false;
	}

	StrUtil::spilt(content, "\n", vecLines);
	return true;
}

bool FileUtil::file_write(const string& file, const string& data)
{
	std::ofstream out;
	if(!open_ofstream(out, file, std::ios::binary)){
		return false;
	} 

	out << data;
	return true;
}

bool FileUtil::file_write(const string& file, unsigned char* data, int len)
{
	std::ofstream out;
	if(!open_ofstream(out, file, std::ios::binary)){
		return false;
	} 

	out.write((char*)data, len);
	return true;
}


#ifdef __LINUX__

void FileUtil::scan_files(vector<string>& vecFiles, const string& path, const string& suffix)  
{  
	DIR *dir;
	struct dirent *ptr;
	char base[1000];
	string suffix2 = suffix;
	StrUtil::removeBeginStr(suffix2, "*.");

	if ((dir=opendir(path.c_str())) == NULL)
	{
		return;
	}

	while ((ptr=readdir(dir)) != NULL)
	{
		if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    //current dir OR parrent dir
		{
			continue;
		}
		else if( (ptr->d_type==8  || ptr->d_type==0)
			&& StrUtil::endWith(string(ptr->d_name), suffix2))    //file
		{
			memset(base, '\0', sizeof(base));
			strcpy(base, path.c_str());
			strcat(base, ptr->d_name);

			vecFiles.push_back(string(base));
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
void FileUtil::scan_files(vector<string>& vecFiles, const string& path, const string& suffix)  
{  
	long   hFile   =   0;  
	struct _finddata_t fileinfo;

	string findStr = path + suffix;
	if((hFile = _findfirst(findStr.c_str(), &fileinfo)) !=  -1)  
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

bool FileUtil::cleanFile(const std::string& filename)
{
	FILE* fd = fopen(filename.c_str(), "w");
	if (fd != NULL)
	{
		fclose(fd);
		return true;
	}
	return false;
}

#if defined(__LINUX__)
bool FileUtil::cleanFile(FILE* file_handle)
{
	fflush(file_handle);
	if (0 == ftruncate(fileno(file_handle), 0))
	{
		rewind(file_handle);
		return true;
	}
	return false;
}
#endif

bool FileUtil::renameFile(const std::string& filename, const std::string& new_filename)
{
	rename(filename.c_str(), new_filename.c_str());
	return true;
}

bool FileUtil::backupFile(const std::string& filename, const std::string& filename_bak)
{
	deleteFile(filename_bak);
	return renameFile(filename, filename_bak);
}

bool FileUtil::backupFile(FILE* file_handle, const std::string& filename, const std::string& filename_bak)
{
	deleteFile(filename_bak);
	if (!file_copy(filename, filename_bak))
	{
		return false;
	}

#ifdef _WIN32
	return cleanFile(filename);
#else
	return cleanFile(file_handle);
#endif
}
