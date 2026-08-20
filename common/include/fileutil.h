#ifndef FILE_UTIL_H__
#define FILE_UTIL_H__

#include <fstream>
#include <vector>

namespace comm
{
class FileUtil
{
public:
	static bool open_ifstream(std::ifstream& stm, const std::string& fileName);
	static bool open_ifstream(std::ifstream& stm, const std::string& fileName,
#if defined(_WIN32) || defined(_WIN32_WCE)
std::ios_base::open_mode mode);
#elif defined(__IPHONEOS__) || defined(ANDROID)
std::ios_base::openmode mode);
#else
std::_Ios_Openmode mode);
#endif
	static bool open_ofstream(std::ofstream& stm, const std::string& fileName);
	static bool open_ofstream(std::ofstream& stm, const std::string& fileName, 
#if defined(_WIN32) || defined(_WIN32_WCE)
std::ios_base::open_mode mode);
#elif defined(__IPHONEOS__) || defined(ANDROID)
std::ios_base::openmode mode);
#else
std::_Ios_Openmode mode);
#endif
	static bool file_exist(const std::string& fileName);
	static std::string getBaseName(const std::string& filename);
	static std::string getExtName(const std::string& filename);
	static int getFileSize(const std::string& filename);
	static int getFileSize(FILE* file_handle);
	static std::string getFileCreateTime(const std::string& filename);
	static std::string getFileModifyTime(const std::string& filename);
	static std::string getFileMd5(const std::string& filename);

	static void deleteFile(const std::string& filename);

	static bool file_copy(const std::string& src, const std::string& dst);
	static std::string file_read(const std::string& file);
	static long file_read(const std::string& file, unsigned char* data, int len);
	static bool file_read_lines(const std::string& file, std::vector<std::string>& vecLines);
	static bool file_write(const std::string& file, const std::string& data);
	static bool file_write(const std::string& file, unsigned char* data, int len);
	

	//return vecFiles with path+filename
	static void scan_files(std::vector<std::string>& vecFiles, const std::string& path, const std::string& suffix);

	static bool cleanFile(const std::string& filename);
#if defined(__LINUX__)
	static bool cleanFile(FILE* file_handle);
#endif
	static bool renameFile(const std::string& filename, const std::string& new_filename);
	//will delete filename
	static bool backupFile(const std::string& filename, const std::string& filename_bak);
	//will not delete filename, but clean it's content
	static bool backupFile(FILE* file_handle, const std::string& filename, const std::string& filename_bak);
};

}

#endif
