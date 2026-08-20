#ifndef dns_soloader_h__
#define dns_soloader_h__

#include "soloader.h"

namespace comm
{

class DnsSoLoader : public comm::SoLoader
{
public:
	DnsSoLoader(const std::string& dll_name, comm::Logger* logger);
	char* mydns_gethostbyname(const char* name, const char* local_if);
	char* mydns_gethostbyname_aaaa(const char* name, const char* local_if);

protected:
	virtual bool loadFuns();

private:
	typedef char* (*mydns_gethostbyname_func)(const char* name, const char* local_if);
	mydns_gethostbyname_func m_func_mydns_gethostbyname;
	mydns_gethostbyname_func m_func_mydns_gethostbyname_aaaa;
};

}
#endif
