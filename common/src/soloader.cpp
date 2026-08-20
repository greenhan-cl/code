#include "soloader.h"
#include "fileutil.h"
#include "pathutil.h"
#include "strutil.h"

#if defined(__LINUX__)
#include <unistd.h>
#include <dlfcn.h>
#endif

using namespace std;
using namespace comm;

SoLoader::SoLoader(const std::string& dll_name, comm::Logger* logger){
	m_dllname = dll_name;
	m_logger = logger;
	m_hDylibHandle = NULL;
}

bool SoLoader::Load()
{
#if defined(_WIN32)
	wchar_t	szCurPath[MAX_PATH];
	GetCurrentDirectoryW(MAX_PATH, szCurPath);

	wstring dllPath = PathUtil::GetExePathW();
	SetCurrentDirectoryW(dllPath.c_str());

	m_hDylibHandle = ::LoadLibraryW(StrUtil::Mbcs2Unicode(m_dllname).c_str());
	if (m_hDylibHandle == NULL)
	{
		//m_logger->logFmt("failed load %s (error=%d)", m_dllname.c_str(), GetLastError());
		return false;
	}
	else
	{
		m_logger->logFmt("load %s success", m_dllname.c_str());
	}
	SetCurrentDirectoryW(szCurPath);
#else
	string dllPath = PathUtil::GetExePathA();
	chdir(dllPath.c_str());

	m_hDylibHandle  = dlopen(m_dllname.c_str(), RTLD_LAZY);
	if(m_hDylibHandle == NULL)
	{
		//m_logger->logFmt("failed load %s (dlerror=%s)", m_dllname.c_str(), dlerror());
		return false;
	}
	else
	{
		m_logger->logFmt("load %s success", m_dllname.c_str());
	}
#endif
	return loadFuns();
}

void SoLoader::UnLoad()
{
	if (NULL != m_hDylibHandle)
	{
#if defined(_WIN32)
		if (!FreeLibrary(m_hDylibHandle)){
			m_logger->logFmt("FreeLibrary error: %d\n", GetLastError());
		}
#else
		if (0 != dlclose(m_hDylibHandle))
		{
			m_logger->logFmt("dlclose error: %s\n", dlerror());
		}
#endif	
	}
} 

void* SoLoader::GetProc(const char* szProcName)
{
	void* pAddress = NULL;

	if (NULL == m_hDylibHandle)
	{
		return pAddress;
	}

	if (NULL == szProcName)
	{
		return pAddress;
	}

#if defined(WIN32)
	pAddress = GetProcAddress(m_hDylibHandle, szProcName);
#else //WIN32
	pAddress = dlsym(m_hDylibHandle, szProcName);
#endif //WIN32

	if (NULL == pAddress)
	{
#if defined(_WIN32)
		m_logger->logFmt("GetProcAddress '%s' error: %d\n", szProcName, GetLastError());
#else
		m_logger->logFmt("dlsym '%s' error: %s\n", szProcName, dlerror());
#endif
	}
	else
	{
		m_logger->logFmt("GetProcAddress '%s' success", szProcName);
	}

	return pAddress;
}
