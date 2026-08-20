#include "dnsutil.h"
#include "iputil.h"

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>

#import <Foundation/Foundation.h>

using namespace std;
using namespace comm;

extern struct __res_state _res;

void DnsUtil::setFirstAddress(const char* ipaddress)
{
    if (m_first_addr.empty() && ipaddress) {
        m_first_addr = ipaddress;
    }
}

bool DnsUtil::resolveHost(const char* host, int resolve_type, int family)
{
    m_domain = host;
    m_v4_list.clear();
    m_v6_list.clear();
    m_first_addr = "";
    
    if (!host || !(*host)) {
        return false;
    }
    
    if (IpUtil::isValidIp(host)) {
        m_v4_list.push_back(host);
        m_first_addr = host;
        return true;
    }
    
    if (IpUtil::isValidIpv6(host)) {
        m_v6_list.push_back(host);
        m_first_addr = host;
        return true;
    }
    
    bool isSuccResolve = false;
    switch (resolve_type) {
            
        case RESOLVE_TYPE_QUERY:
            isSuccResolve = resolveHostByQuery(host, family);
            break;
            
        case RESOLVE_TYPE_IOS:
            isSuccResolve = resolveHostByIos(host);
            break;
            
        case RESOLVE_TYPE_LOCAL:
        default:
            isSuccResolve = resolveHostByLocal(host, family);
            break;
    }
    
    if (m_v4_list.empty() && m_v6_list.empty()) {
        return false;
    }
    
    return isSuccResolve;
}

bool DnsUtil::resolveHostByLocal(const char* host, int family)
{
    char buffer[INET6_ADDRSTRLEN];
    struct addrinfo *result = NULL, *ptr = NULL;
    struct addrinfo hints;
    
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = family;
    hints.ai_flags = AI_ADDRCONFIG; //AI_ALL
    hints.ai_protocol = 0;
    hints.ai_socktype = SOCK_STREAM;
    
    int ret = getaddrinfo(host, 0, &hints, &result);
    if (ret != 0 || result == NULL) {
        return false;
    }
    
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        memset(&buffer, 0, INET6_ADDRSTRLEN);
        if (ptr->ai_family == AF_INET)
        {
            struct sockaddr_in *addr = reinterpret_cast<struct sockaddr_in *>(ptr->ai_addr);
            if (NULL != inet_ntop(AF_INET, &addr->sin_addr, buffer, INET6_ADDRSTRLEN) && *buffer)
            {
                m_v4_list.push_back(buffer);
            }
        }
        if (ptr->ai_family == AF_INET6)
        {
            struct sockaddr_in6 *addr = reinterpret_cast<struct sockaddr_in6 *>(ptr->ai_addr);
            if (NULL != inet_ntop(AF_INET6, &addr->sin6_addr, buffer, INET6_ADDRSTRLEN) && *buffer)
            {
                m_v6_list.push_back(buffer);
            }
        }
        setFirstAddress(buffer);
    }
    
    freeaddrinfo(result);
    return true;
}

bool DnsUtil::resolveHostByQuery(const char* host, int family)
{
    if(res_init() != 0 ) { /* read config /etc/resolv.conf */
        return false;
    }
    
//    _res.options = RES_DEFAULT;       //all-or-none
//    _res.retry = 1;
//    _res.nscount = 1;
//    _res.nsaddr_list[0].sin_addr.s_addr = inet_addr("8.8.8.8");
    
    ns_type query_type = ns_t_a;
    switch (family) {
        case AF_INET6:
            query_type = ns_t_aaaa;
            break;
            
        default:
            break;
    }
    
    unsigned char auResult[512];
    char ipStr[INET6_ADDRSTRLEN];
    int nBytesRead = res_query(host, ns_c_in, query_type, auResult, sizeof(auResult));
    
    ns_msg handle;
    ns_initparse(auResult, nBytesRead, &handle);
    
    int msg_count = ns_msg_count(handle, ns_s_an);
    if (msg_count > 0) {
        for(int rrnum = 0; rrnum < msg_count; rrnum++) {
            ns_rr rr;
            if(ns_parserr(&handle, ns_s_an, rrnum, &rr) == 0) {
                memset(&ipStr, 0, INET6_ADDRSTRLEN);
                unsigned short rr_type = ns_rr_type(rr);
                if (rr_type == ns_t_a)  //v4
                {
                    if (NULL != inet_ntop(AF_INET, (struct in_addr *)ns_rr_rdata(rr), ipStr, INET6_ADDRSTRLEN) && *ipStr)
                    {
                        m_v4_list.push_back(ipStr);
                    }
                }
                else if (rr_type == ns_t_aaaa) //v6
                {
                    if (NULL != inet_ntop(AF_INET6, (struct in6_addr *)ns_rr_rdata(rr), ipStr, INET6_ADDRSTRLEN) && *ipStr)
                    {
                        m_v6_list.push_back(ipStr);
                    }
                }
                setFirstAddress(ipStr);
            }
        }
    }
    return true;
}

bool DnsUtil::resolveHostByIos(const char* host)
{
    CFArrayRef addresses;
    NSString *ipAddress = nil;
    CFHostRef hostRef = CFHostCreateWithName(kCFAllocatorDefault, (__bridge CFStringRef)[NSString stringWithUTF8String:host]);
    Boolean result = CFHostStartInfoResolution(hostRef, kCFHostAddresses, NULL); // pass an error instead of NULL here to find out why it failed
    if (result == TRUE) {
        addresses = CFHostGetAddressing(hostRef, &result);
    }
    if (result == TRUE) {
        for (CFIndex index = 0; index<CFArrayGetCount(addresses) ; index++)
        {
            CFDataRef ref = (CFDataRef) CFArrayGetValueAtIndex(addresses, index);
            struct sockaddr *addressGeneric;
            NSData *myData = (__bridge NSData *)ref;
            addressGeneric = (struct sockaddr *)[myData bytes];

            switch (addressGeneric->sa_family) {
                case AF_INET: {
                    struct sockaddr_in *ip4;
                    char dest[INET_ADDRSTRLEN];
                    ip4 = (struct sockaddr_in *)[myData bytes];
                    //port = ntohs(ip4->sin_port);
                    ipAddress = [NSString stringWithFormat:@"%s", inet_ntop(AF_INET, &ip4->sin_addr, dest, sizeof dest)];
                    m_v4_list.push_back([ipAddress UTF8String]);
                }
                    break;

                case AF_INET6: {
                    struct sockaddr_in6 *ip6;
                    char dest[INET6_ADDRSTRLEN];
                    ip6 = (struct sockaddr_in6 *)[myData bytes];
                    //port = ntohs(ip6->sin6_port);
                    ipAddress = [NSString stringWithFormat:@"%s", inet_ntop(AF_INET6, &ip6->sin6_addr, dest, sizeof dest)];
                    m_v6_list.push_back([ipAddress UTF8String]);
                }
                    break;

                default:
                    ipAddress = nil;
                    break;
            }

            setFirstAddress([ipAddress UTF8String]);
        }
    }
    CFRelease(hostRef);
    return (result == TRUE);
}
