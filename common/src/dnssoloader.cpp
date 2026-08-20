#include "dnssoloader.h"

using namespace std;
using namespace comm;

DnsSoLoader::DnsSoLoader(const std::string& dll_name, comm::Logger* logger)
:SoLoader(dll_name, logger)
{
	m_func_mydns_gethostbyname = NULL;
	m_func_mydns_gethostbyname_aaaa = NULL;
}

bool DnsSoLoader::loadFuns()
{
	m_func_mydns_gethostbyname = (mydns_gethostbyname_func) GetProc("mydns_gethostbyname");
	m_func_mydns_gethostbyname_aaaa = (mydns_gethostbyname_func) GetProc("mydns_gethostbyname_aaaa");
	if (m_func_mydns_gethostbyname==NULL || m_func_mydns_gethostbyname_aaaa==NULL)
	{
		m_logger->log("GetProc mydns_gethostbyname failed");
		return false;
	}
	return true;
}

char* DnsSoLoader::mydns_gethostbyname(const char* name, const char* local_if)
{
	if(m_func_mydns_gethostbyname == NULL){
		return NULL;
	}

	//call it
	return (*m_func_mydns_gethostbyname)(name, local_if);
}

char* DnsSoLoader::mydns_gethostbyname_aaaa(const char* name, const char* local_if)
{
	if(m_func_mydns_gethostbyname_aaaa == NULL){
		return NULL;
	}

	//call it
	return (*m_func_mydns_gethostbyname_aaaa)(name, local_if);
}
