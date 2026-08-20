#ifndef SO_LOADER_H__
#define SO_LOADER_H__

#include "platinc.h"
#include "logger.h"
#include <string>

namespace comm
{

#if defined(_WIN32)
	#define DL_HANDLE HMODULE
#else
	#define DL_HANDLE void*
#endif

class SoLoader
{
public:
	SoLoader(const std::string& dll_name, comm::Logger* logger);
	bool Load();
	void UnLoad();

protected:
	virtual bool loadFuns()=0;
	void* GetProc(const char* szProcName);

protected:
	std::string m_dllname;
	comm::Logger* m_logger;
	DL_HANDLE m_hDylibHandle;	
};

}
#endif
