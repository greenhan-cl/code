#include "mysock_define.h"
#include "mysock.h"
#include "common.h"
#include "iputil.h"
#include "dnssoloader.h"

#ifdef _WIN32
#include <ws2tcpip.h>
#else
#include<netdb.h>
#include <ifaddrs.h>
#endif

using namespace std;

namespace comm
{
static char g_socket_error[200];
static char g_resolv_ip[60];

static comm::Logger* g_logger=NULL;
static bool g_logger_newlocal = false;
static int g_socket_read_timeout_ms = 10 * 1000;
static int g_socket_write_timeout_ms = 10 * 1000;
static bool g_inited = false; //single instance

int mysock_init()
{
	if(g_inited){
		return 0;
	}

#if defined(_WIN32)
	WSADATA wsaData;
	int res = WSAStartup (MAKEWORD (2,2), &wsaData);
	if (res != NO_ERROR) 
	{
		return -1;
	}
#endif

#if defined(ANDROID)
	g_logger = new Logger("/mnt/sdcard/ipc2/mysock.log");
#else
    g_logger = new Logger(PathUtil::GetLogPath()+"mysock.log");
#endif
	g_logger->log("mysock_init");
    g_logger_newlocal = true;
	g_inited = true;
	return 0;
}
    
bool mysock_reset_logger(void* logger)
{
    if(g_inited){
        g_logger = (comm::Logger*)logger;
        return true;
    }
    return false;
}

int mysock_init_with_logger(void* logger)
{
	g_logger = (comm::Logger*)logger;
	if(g_inited){
		return 0;
	}

#if defined(_WIN32) || defined(_WIN32_WCE)
	WSADATA wsaData;
	int res = WSAStartup (MAKEWORD (2,2), &wsaData);
	if (res != NO_ERROR) 
	{
		return -1;
	}
#endif

	g_logger->log("mysock_init_with_logger");
	g_inited = true;
	return 0;
}

void mysock_uninit()
{
	if (g_logger != NULL) {
		g_logger->log("mysock_uninit");
	}
	return; //do nothing, not need release

	/*
#if defined(_WIN32) || defined(_WIN32_WCE)
	WSACleanup();
#endif
	*/
}

//AF_INET, SOCK_STREAM, IPPROTO_TCP
int mysock_socket(const int af,const int type,const int protocol)
{
	int fd = socket(af, type, protocol);
	return fd;
}

void mysock_readtimeout_set(int timeout_ms)
{
	g_socket_read_timeout_ms = timeout_ms;
}

void mysock_writetimeout_set(int timeout_ms)
{
	g_socket_write_timeout_ms = timeout_ms;
}

int mysock_writen(const int fd, unsigned char* indata, int indata_len, int *conn_break)
{
	int rc = -1;
	int nright;
	int nwritten = 0;
	char* ptr;
	
	ptr = (char*)indata;
	nright = indata_len;
	while(nright > 0)
	{
		if((rc = mysock_select(fd+1, 0, g_socket_write_timeout_ms, conn_break)) < 0) 
		{
			g_logger->logFmt("mysock_writen mysock_select rc=%d",rc);
			return rc;
		}

#if defined(_WIN32) || defined(_WIN32_WCE) || defined(__IPHONEOS__) || defined(__SYMBIAN32__)
		nwritten = send(fd,  ptr, nright, 0);
#else
		nwritten = write(fd, ptr, nright);
#endif
		if(nwritten < 0)
		{
			int err_code = mysock_error_code();
			g_logger->logFmt("mysock_writen send nwritten=%d error=%s",nwritten, mysock_error());
			
			if(mysock_noblock_error(err_code) == 0)
			{
				nwritten = 0; /*and call write() again*/					
			}
			else
			{
				return nwritten;
			}
		}
		else if (nwritten == 0)
		{
			return nwritten;
		}
		
		nright -= nwritten;
		ptr += nwritten;
	}
	return indata_len;
}

int mysock_readn(const int fd, unsigned char* outdata, int want_datalen, int *conn_break)
{
	int rc = -1;
	int nright;
	int nread = 0;
	char* ptr;
	ptr = (char*)outdata;
	nright = want_datalen;

	while(nright > 0)
	{
		if((rc = mysock_select(fd+1, 1, g_socket_read_timeout_ms, conn_break)) < 0) 
		{
			g_logger->logFmt("mysock_readn select rc=%d",rc);
			return rc;
		}

#if defined(_WIN32) || defined(_WIN32_WCE) || defined(__IPHONEOS__) || defined(__SYMBIAN32__)
		nread = recv(fd, ptr, nright, 0);
#else
		nread = read(fd, ptr, nright);
#endif
		if (nread < 0)
		{
			int err_code = mysock_error_code();
			g_logger->logFmt("mysock_readn recv nread=%d error=%s", nread, mysock_error());

			if(mysock_noblock_error(err_code) == 0)
			{
				nread = 0; /*and call read() again*/			
			}
			else 
			{			
				return nread; /*error*/
			}
		}
		else if(nread == 0)
		{
			return nread; /*EOF*/
		}
		
		nright -= nread;
		ptr += nread;
	}
	
	return want_datalen - nright;
}

int mysock_read(const int fd, unsigned char* outdata, int want_datalen, int* conn_break)
{
	int rc = -1;
	int nread = 0;
	while (1)
	{
		if((rc = mysock_select(fd+1, 1, g_socket_read_timeout_ms, conn_break)) < 0) 
		{
			g_logger->logFmt("mysock_read select error:%d",rc);
			return rc;
		}

#if defined(_WIN32) || defined(_WIN32_WCE) || defined(__IPHONEOS__) || defined(__SYMBIAN32__)
		nread = recv(fd, (char*)outdata, want_datalen, 0);
#else
		nread = read(fd, outdata, want_datalen);
#endif
		if (nread < 0)
		{
			int err_code = mysock_error_code();
			g_logger->logFmt("mysock_read recv nread=%d error=%s", nread, mysock_error());

			if(mysock_noblock_error(err_code) == 0)
				continue; /*and call read() again*/
			else
				break; /*error*/
		}
		else if(nread == 0)
		{
			break; /*EOF*/
		}
		else
		{
			break; /*ok, read once*/
		}
	}

	return nread;
}

int mysock_select(const int max_fd, int read_socket,  const int timeout_ms, int *break_flag)
{
	int r = 0;
	int fd = max_fd-1;
	int wait_per_ms = 1000;
	int wait_count = timeout_ms / wait_per_ms;
	wait_count = wait_count > 0 ? wait_count : 1;

	for(int i = 0; i < wait_count; i++)
	{	
		fd_set fs;
		FD_ZERO(&fs);
		FD_SET(fd, &fs);

		timeval timeout;
		timeout.tv_sec = (long)(wait_per_ms / 1000);
		timeout.tv_usec = (long)((wait_per_ms % 1000) * 1000);

		if(read_socket)
		{
			r = select(fd+1, &fs, NULL, NULL, &timeout);
		}
		else
		{
			r = select(fd+1, NULL, &fs, NULL, &timeout);
		}

		if (r == 0)
		{
			if (*break_flag)
			{
				return -2;
			}
			else
			{
				continue; //timeout
			}
		}
		else if (r < 0)
		{
			return -3; //failed
		}
		else
		{
			break; //ok
		}
	}

	if (r == 0) //timeout
	{
		return -4;
	}

	return r;
}

//linux: local_if->dev, win32: local_if->local_ip
int mysock_bind(const int fd, const char* local_if, int local_port)
{
	char local_ip[60];
	struct sockaddr_in local;
	memset(&local, 0, sizeof(local));
	if( (local_if==NULL || !(*local_if)) && local_port==0 )
	{
		return -1; //not bind
	}

	memset(local_ip, 0x00, sizeof(local_ip));

	if (local_if!=NULL && *local_if)
	{
		//判断是否为IP地址。
		if (inet_addr(local_if) != INADDR_NONE)
		{
			strncpy(local_ip, local_if, strlen(local_if));	
		}
		else
		{
			char* ip = mysock_if2ip(local_if);
			if (ip == NULL)
			{
				g_logger->logFmt("mysock_if2ip failed. local_if=%s", local_if);
				return -1;
			}
			else
			{
				g_logger->logFmt("mysock_if2ip %s=%s", local_if, ip);
			}

			strncpy(local_ip, ip, sizeof(local_ip));

	#if defined(__LINUX__) && !defined(__IPHONEOS__)
			/* I am not sure any other OSs than Linux that provide this feature, and
			* at the least I cannot test. --Ben
			*
			* This feature allows one to tightly bind the local socket to a
			* particular interface.  This will force even requests to other local
			* interfaces to go out the external interface.
			*
			*
			* Only bind to the interface when specified as interface, not just as a
			* hostname or ip address.
			*/
			struct ifreq ifr;
			memset(&ifr, 0, sizeof(ifr));
			strncpy(ifr.ifr_name, local_if, IFNAMSIZ);
			if(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr, sizeof(ifr)) != 0) 
			{
				g_logger->log("setsockopt SO_BINDTODEVICE failed");
				return -1;
			}
	#endif
		}
	}

	local.sin_family = AF_INET;
	local.sin_port = htons(local_port);
	if (local_ip && *local_ip)
	{
		local.sin_addr.s_addr = inet_addr( local_ip );			
	}
	else
	{
		local.sin_addr.s_addr = htonl(INADDR_ANY);
	}

    if( ::bind( fd, (struct sockaddr *) &local, sizeof( struct sockaddr_in ) ) == -1 )
	{
		g_logger->logFmt("bind error=%s", mysock_error());
		return -1;
	}

	return 0;
}

int mysock_error_code()
{
	int err = 0;
#if defined(_WIN32) || defined(_WIN32_WCE)
	err = WSAGetLastError( );
#else
	err = errno;
#endif

	return err;
}

const char* mysock_error()
{
	int err_code = mysock_error_code();

#if defined(_WIN32) || defined(_WIN32_WCE)
	sprintf(g_socket_error, "%d", err_code);
#else
	sprintf(g_socket_error, "%s(%d)", strerror(err_code), err_code);
#endif

	return g_socket_error;
}

void mysock_set_errcode(int err)
{
#if defined(_WIN32) || defined(_WIN32_WCE)
	WSASetLastError(err);
#else
	errno = err;
#endif
}

int mysock_noblock_error(int err)
{
#if defined(_WIN32) || defined(_WIN32_WCE) 
	if ( err == WSAEWOULDBLOCK || err == WSAEINTR)
#else
	if ( err ==EWOULDBLOCK || err==EAGAIN || err == EINTR) 
#endif
	{
		return 0;
	}

	return -1;
}

void mysock_close(const int fd)
{
	if (fd < 0)
	{
		return;
	}

#if defined(_WIN32) || defined(_WIN32_WCE)
	closesocket (fd);
#else
	close(fd);
#endif
}

int mysock_noblock(const int fd, int noblock)
{
#if defined(_WIN32) || defined(_WIN32_WCE)
	unsigned long flags = noblock ? 1 : 0; //阻塞为0 非阻塞为1
	ioctlsocket(fd, FIONBIO, &flags);
#else
	int flags;
	flags = fcntl(fd, F_GETFL, 0);
	if(noblock)
	{
		fcntl(fd, F_SETFL, flags|O_NONBLOCK);
	}
	else
	{
		fcntl(fd, F_SETFL, flags&(~O_NONBLOCK));
	}
#endif
	return 0;
}

int mysock_option_nodelay(const int fd)
{
	int nodelay = 1;
	return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char*)&nodelay, sizeof(nodelay));
}

int mysock_option_send_windowsize(const int fd, int win_size)
{
	return setsockopt( fd, SOL_SOCKET, SO_SNDBUF, (const char*)&win_size, sizeof(win_size) );
}

int mysock_option_recv_windowsize(const int fd, int win_size)
{
	return setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*)&win_size, sizeof(win_size));
}

int mysock_option_reuse(const int fd)
{
	int reuse  = 1;
	return setsockopt( fd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof( reuse ) );
}

int mysock_option_get_soerror(const int fd)
{
	int err = 0;

#if defined(ANDROID)
	int err_size = sizeof(err);
	if(0 != getsockopt(fd, SOL_SOCKET, SO_ERROR, (char *)&err, (socklen_t*)&err_size))
#else
	SOCKLEN_T err_size = sizeof(err);
	if(0 != getsockopt(fd, SOL_SOCKET, SO_ERROR, (char *)&err, &err_size))
#endif

	/**
	 * normal getsockopt will return 0 and set error at err
	 * but Solaris getsockopt return -1 and set error at errno
	 */
	{
		err = mysock_error_code();
		g_logger->logFmt("getsockopt!=0 use mysock_error_code err=%d", err);
	}
	else
	{
		//test code (win32, rcu)
		//int now_errno = errno;
		//g_logger->logFmt("getsockopt==0 err=%d errno=%d", err, now_errno);
	}

	return err;
}

int mysock_connect_ipport(const int fd, const char* serv_ip, int serv_port)
{
	int rc = 0;

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(serv_port);
	addr.sin_addr.s_addr = inet_addr(serv_ip);	
	rc = connect(fd, (struct sockaddr *)&addr, sizeof(addr));

	return rc;
}

int mysock_connect_ipport_v6(const int fd, const char* serv_ip, int serv_port)
{
	int rc = 0;

	struct sockaddr_in6 addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin6_family = AF_INET6;
	addr.sin6_port = htons(serv_port);

	if ( inet_pton(AF_INET6, serv_ip, &addr.sin6_addr) < 0 ) {
		return -1;
	}
	
	rc = connect(fd, (struct sockaddr *)&addr, sizeof(addr));

	return rc;
}

//r=pre connect ret
int mysock_wait_connect(const int fd, int r, int timeout_ms, int* conn_break)
{
	int so_err = 0;
	return mysock_wait_connect2(fd, r, timeout_ms, conn_break, &so_err);
}

int mysock_wait_connect2(const int fd, int r, int timeout_ms, int* conn_break, int* so_err)
{	
	int err = 0;
	if (r == 0)
	{
		/* connected immediately */
		return 0;
	}
	else if (r == -1) 
	{
		err = mysock_error_code();
#if defined(_WIN32) || defined(_WIN32_WCE) 
		if ( err == WSAEINPROGRESS || err == WSAEWOULDBLOCK) 
#else
		if ( err == EINPROGRESS || err == EAGAIN) 
#endif
		{
			/* select */
		}
		else 
		{
			*so_err = mysock_error_code(); //use sock error as so error
			g_logger->logFmt("mysock_wait_connect pre failed. so_err=%d(%s)", *so_err, mysock_error());
			return -1;
		}
	}

	fd_set wfds;
	fd_set rfds;
	struct timeval timeout;

	int wait_per_ms = 1000;
	int wait_count = timeout_ms / wait_per_ms;
	wait_count = wait_count > 0 ? wait_count : 1;

	for(int i = 0; i < wait_count; i++)
	{	
		FD_ZERO(&wfds);
		FD_SET(fd, &wfds);
		rfds = wfds;

		timeout.tv_sec = (long)(wait_per_ms / 1000);
		timeout.tv_usec = (long)((wait_per_ms % 1000) * 1000);

		r = select(fd + 1, &rfds, &wfds, NULL, &timeout);
		if (r == 0)
		{
			if (*conn_break)
			{
				return -2;
			}
			else
			{
#if defined(WIN32)
				//WIN32在有错误时(本地网络断开、对方RST拒绝等), rfds不会可读但socket错误码会设置。
				//其他平台会rfds可读，再判断socket错误码。
				err = mysock_option_get_soerror(fd);
				if(err > 0)
				{
					g_logger->logFmt("select 1s timeout. so_err=%d", err);
					*so_err = err;
					return -1; //failed
				}
#endif
				continue; //this timeout
			}
		}
		else if (r < 0)
		{
			return -3; //failed
		}
		else
		{
			break; //ok
		}
	}

	if (r == 0) //timeout
	{
		g_logger->log("mysock_wait_connect timeout.");
		return -4;
	}
	
	if (!FD_ISSET(fd, &rfds) && !FD_ISSET(fd, &wfds))
	{
		err = mysock_option_get_soerror(fd);
		g_logger->logFmt("socketfd not set? can't be here. r=%d err=%d", r, err);
		return -6;
	}

	err = mysock_option_get_soerror(fd);

#if defined(WIN32)
	if((0 == err) 	|| (WSAEISCONN == err))
#else
	if((0 == err) 	|| (EISCONN == err))
#endif
	{
		/* we are connected*/
		return 0;
	}
	else
	{
		*so_err = err;
		g_logger->logFmt("mysock_wait_connect failed. so_err=%d", *so_err);
		return -1;
	}
}

std::string mysock_gethostbynamebase_str(const char* name, const char* local_if, int ip_type)
{
	if (!name || !(*name) )
	{
		return "";
	}

	if (!IpUtil::isValidDomain(name))
	{
		return name;
	}

	char buffer[INET6_ADDRSTRLEN];
	struct addrinfo *result = NULL;
	struct addrinfo hints;
	string host_ip = "";
			
	memset(&buffer, 0, INET6_ADDRSTRLEN);
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = ip_type; 
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 0; 
	hints.ai_socktype = SOCK_STREAM;
	
	int ret = getaddrinfo(name, 0, &hints, &result);
	if (ret != 0 || result == NULL) {
		g_logger->logFmt("getaddrinfo failed! name=%s(use sysdns)", name);
		return "";
	}
	else {
		//for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
		if (ip_type == AF_INET)
		{
			 struct sockaddr_in *addr = (struct sockaddr_in *)result->ai_addr;
			 if (NULL != inet_ntop(AF_INET, &addr->sin_addr, buffer, INET6_ADDRSTRLEN))
			 {
				 host_ip = buffer;
			 }
		}
		if (ip_type == AF_INET6)
		{
			struct sockaddr_in6 *addr = (struct sockaddr_in6 *) result->ai_addr;
			if (NULL != inet_ntop(AF_INET6, &addr->sin6_addr, buffer, INET6_ADDRSTRLEN))
			{
				host_ip = buffer;
			}
		}
		freeaddrinfo(result);

		/*
		if (host_ip == "")
		{
			g_logger->logFmt("gethostbyname failed! name=%s(use mydns)", name);
		}
		else
		{
			g_logger->logFmt("gethostbyname success %s=%s(use sysdns)", name, host_ip.c_str());
		}*/
	}

	return host_ip;
}

char* mysock_gethostbyname(const char* name, const char* local_if)
{
	string host_ip = mysock_gethostbynamebase_str(name, local_if, AF_INET);
	strncpy(g_resolv_ip, host_ip.c_str(), sizeof(g_resolv_ip));
	g_resolv_ip[sizeof(g_resolv_ip)-1] = '\0';
	return g_resolv_ip;
}

char* mysock_gethostbyname_aaaa(const char* name, const char* local_if)
{
	string host_ip = mysock_gethostbynamebase_str(name, local_if, AF_INET6);
	strncpy(g_resolv_ip, host_ip.c_str(), sizeof(g_resolv_ip));
	g_resolv_ip[sizeof(g_resolv_ip)-1] = '\0';
	return g_resolv_ip;
}

std::string mysock_gethostbyname_str(const char* name, const char* local_if)
{
	return mysock_gethostbynamebase_str(name, local_if, AF_INET);
}

std::string mysock_gethostbyname_aaaa_str(const char* name, const char* local_if)
{
	return mysock_gethostbynamebase_str(name, local_if, AF_INET6);
}

int mysock_listen(const int fd, int queSize)
{
	return listen( fd, queSize);
}

int mysock_accept(const int fd, int timeout_ms, int* conn_break)
{
	int ret = 0;

	ret = mysock_select(fd+1, 1, timeout_ms, conn_break);
	if ( ret < 0)
	{
		return ret;
	}

	struct sockaddr_in addr;
	memset(&addr, 0x00, sizeof(addr));

	#if defined(ANDROID)
		int addrlen = (int)sizeof( addr );
		return accept(fd, (struct sockaddr* )&addr, (socklen_t*)&addrlen );
	#else
		SOCKLEN_T addrlen = (SOCKLEN_T)sizeof( addr );
		return accept(fd, (struct sockaddr* )&addr, &addrlen );
	#endif
}

int mysock_getsockname(const int fd, char **local_if, int *local_port)
{
	int rc = 0;

	struct sockaddr_in nameaddr;
	memset(&nameaddr, 0x00, sizeof(nameaddr));


#if defined(ANDROID)
	int namelen = sizeof( nameaddr );
	if( getsockname( fd, (struct   sockaddr*)&nameaddr,(socklen_t*)&namelen ) )
#else
	SOCKLEN_T namelen = sizeof( nameaddr );
	if( getsockname( fd, (struct   sockaddr*)&nameaddr, &namelen ) )
#endif
	//if( getsockname( fd, (struct   sockaddr*)&nameaddr, &namelen ) )
	{
		rc = -1;
	}
	else
	{
		*local_if = inet_ntoa( nameaddr.sin_addr);
		*local_port = htons( nameaddr.sin_port ) + 1;
	}	
	return rc;
}

int mysock_can_useport(const char* ip, int portMin, int portMax)
{
	int fd;

	if((fd = mysock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
	{
		return -1;
	}

	struct sockaddr_in local;
	memset(&local, 0x00, sizeof(local));
	local.sin_family = AF_INET;
	if (ip && *ip)
	{
		local.sin_addr.s_addr = inet_addr( ip );			
	}
	else
	{
		local.sin_addr.s_addr = htonl(INADDR_ANY);
	}

	for (int i=portMin; i<=portMax; i++)
	{
		local.sin_port = htons(i);
        if( ::bind( fd, (struct sockaddr *) &local, sizeof( struct sockaddr_in ) ) == 0 )
		{
			//found can-use port i
			mysock_close(fd);
			return i;
		}
	}

	//no can-use port 
	mysock_close(fd);
	return -1;
}

char* mysock_if2ip(const char *local_if)
{
	if(!local_if || !(*local_if))
	{
		return NULL;
	}

	//判断是否为IP地址。
	if (inet_addr(local_if) != INADDR_NONE)
	{
		return (char*)local_if;
	}
	else
	{
	#if defined(__LINUX__)
		struct ifreq req;
		struct in_addr in;
		struct sockaddr_in *s;
		int dummy;
		size_t len;
		char *ip;

		len = strlen(local_if);
		if(len >= sizeof(req.ifr_name))
			return NULL;

		dummy = socket(AF_INET, SOCK_STREAM, 0);
		if(-1 == dummy) 
		{
			g_logger->logFmt("mysock_if2ip socket failed. local_if=%s", local_if);
			return NULL;
		}

		memset(&req, 0, sizeof(req));
		memcpy(req.ifr_name, local_if, len+1);
		req.ifr_addr.sa_family = AF_INET;

		if(ioctl(dummy, SIOCGIFADDR, &req) < 0) 
		{
			g_logger->logFmt("mysock_if2ip ioctl failed. local_if=%s", local_if);
			close(dummy);
			return NULL;
		}

		s = (struct sockaddr_in *)&req.ifr_addr;
		memcpy(&in, &s->sin_addr, sizeof(in));
		ip = (char *) inet_ntoa(in);
		close(dummy);
		return ip;
	#else
		g_logger->logFmt("mysock_if2ip failed. local_if=%s", local_if);
		return NULL;
	#endif	
	}
}

std::string mysock_if2ipv6(const char *local_if)
{
	if(!local_if || !(*local_if))
	{
		return "";
	}

#if defined(__LINUX__)
	struct ifaddrs *interfaces = NULL;
	struct ifaddrs *addr = NULL;
	char ip[INET6_ADDRSTRLEN];

	memset(ip, 0x00, sizeof(ip));
	int result = getifaddrs(&interfaces);
	if (result == 0) {
		for (addr = interfaces; addr != NULL; addr = addr->ifa_next) {
			if (addr->ifa_addr && (addr->ifa_flags & IFF_UP) && (addr->ifa_flags & IFF_RUNNING)) 
			{
				void *in_addr;
				if (addr->ifa_addr->sa_family == AF_INET6 && strcmp(addr->ifa_name, local_if) == 0)
				{
					struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)addr->ifa_addr;
					in_addr = &(ipv6->sin6_addr);
				}
				else 
				{
					continue;
				}

				inet_ntop(addr->ifa_addr->sa_family, in_addr, ip, INET6_ADDRSTRLEN);
				break;
			}
		}//for
		freeifaddrs(interfaces);
	}
	else{
		g_logger->logFmt("getifaddrs failed. local_if=%s", local_if);
	}
	return ip;

#else
	g_logger->logFmt("mysock_if2ipv6 failed. local_if=%s", local_if);
	return "";
#endif	
}

void mysock_release_mutex()
{
}
}
