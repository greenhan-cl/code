#ifndef IP_UTIL_H__
#define IP_UTIL_H__

#include "common.h"
namespace comm
{

class IpUtil
{
public:
	static bool isInnerIp(const std::string& strIP);
	static bool isValidIp(const char* ip);
	static bool isValidIpv6(const char* ip);
	static bool isValidDomain(const char* domain);
	static std::string iptostr(unsigned int ip);
	static unsigned int strtoip(const std::string& ip_str);

	static std::string fullIpv6(const std::string& ip_str);

private:
	static void adjustIP(std::string& paraStrIP);
	static void fullIpv6Seg(std::string& seg_str);
};

}
#endif
