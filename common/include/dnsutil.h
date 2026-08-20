#ifndef dnsutil_h
#define dnsutil_h

#include "common.h"

#define RESOLVE_TYPE_LOCAL 0x1      /* AF_UNSPEC = AI_ADDRCONFIG */
#define RESOLVE_TYPE_QUERY 0x2      /* AF_UNSPEC = AF_INET */
#define RESOLVE_TYPE_IOS 0x3        /* AF_UNSPEC = INVAILD */

namespace comm
{
    class DnsUtil{
    public:
        /* family = AF_UNSPEC | AF_INET4 | AF_INET6 */
        bool resolveHost(const char* host, int resolve_type = RESOLVE_TYPE_LOCAL, int family = AF_UNSPEC);
        bool isSupportV4() {return !m_v4_list.empty();}
        bool isSupportV6() {return !m_v6_list.empty();}
        std::string& getDomain() {return m_domain;}
        std::string& getFirstResolve() {return m_first_addr;}
        
        std::string getAddressInfo() {
            std::string info = "Host: " + m_domain + "\n";
            std::list<std::string>::iterator iter = m_v4_list.begin();
            while (iter != m_v4_list.end())
            {
                info += "A: " + (*iter) + "\n";
                iter++;
            }
            
            iter = m_v6_list.begin();
            while (iter != m_v6_list.end())
            {
                info += "AAAA: " + (*iter) + "\n";
                iter++;
            }
            return info;
        }
        
        void toString() //debug
        {
            printf("domain\t: %s\nfirst_ip: %s\n", m_domain.c_str(), m_first_addr.c_str());
            std::list<std::string>::iterator iter = m_v4_list.begin();
            while (iter != m_v4_list.end())
            {
                printf("A\t\t: %s\n", (*iter).c_str());
                iter++;
            }
            
            iter = m_v6_list.begin();
            while (iter != m_v6_list.end())
            {
                printf("AAAA\t: %s\n", (*iter).c_str());
                iter++;
            }
        }
        
    private:
        bool resolveHostByLocal(const char* host, int family);    /* Use Getaddrinfo System Function */
        bool resolveHostByQuery(const char* host, int family);    /* Use res_query System Function */
        bool resolveHostByIos(const char* host);      /* Apple's original DNS analysis */
        void setFirstAddress(const char* ipaddress);
        //other method : gethostname, mydns, httpdns
        
    private:
        std::string m_first_addr;
        std::string m_domain;
        std::list<std::string> m_v4_list;
        std::list<std::string> m_v6_list;
    };
}

#endif /* dnsutil_h */
