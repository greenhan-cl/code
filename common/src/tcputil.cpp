#include "tcputil.h"
#include "mysock_define.h"
#include "mysock.h"
#include "common.h"
#include "iputil.h"
#include <chrono>
using namespace std;
using namespace comm;

int TcpUtil::tcp_connect(const char *serv_ip, int serv_port, const char* local_if, int send_buff_size, int recv_buff_size, int timeout, comm::Logger* logger, int* conn_break, char* err_info)
{
	double take_ms = 0;
	return tcp_connect_take(serv_ip, serv_port, local_if, send_buff_size, recv_buff_size, timeout, logger, conn_break, err_info, take_ms);
}

int TcpUtil::tcp_connect_take(const char *serv_ip, int serv_port, const char* local_if, int send_buff_size, int recv_buff_size, int timeout, comm::Logger* logger, int* conn_break, char* err_info, double& take_ms)
{
	int fd;
	bool ipv6 = IpUtil::isValidIpv6(serv_ip);

	logger->logFmt("TcpUtil::tcp_connect host=%s port=%d", serv_ip, serv_port);
	int af = ipv6 ? AF_INET6 : AF_INET;

	if((fd = mysock_socket(af, SOCK_STREAM, IPPROTO_TCP) ) == -1)
	{
		logger->log("socket failed");
		return ETCP_OTHER;
	}	

	socket_buff_set(fd, send_buff_size, recv_buff_size, logger);

	bool port_bind = false;
	int env_local_port = StrUtil::Str2Int(SysUtil::getEnv("DT_LOCAL_PORT"));
	int env_port_range = StrUtil::Str2Int(SysUtil::getEnv("DT_LOCAL_PORTRANGE"));
	if(env_local_port>0 && env_port_range>0)
	{
		for(int i=0; i<env_port_range; i++)
		{
			int local_port = env_local_port + i;
			if(mysock_bind(fd, local_if,  local_port) == 0)
			{
				port_bind = true;
				break;
			}
		}

		if(!port_bind)
		{
			logger->logFmt("socket_port_bind failed.err=%s", mysock_error());
			mysock_close( fd );
			return ETCP_BIND_FAILED;
		}
	}
	else
	{
		//绑定接口,用于多路
		if(local_if!=NULL && *local_if)
		{
			logger->logFmt("socket_bind local_if:%s", local_if);
			if(mysock_bind(fd, local_if, 0) == -1)
			{
				logger->logFmt("socket_bind failed.err=%s", mysock_error());
				mysock_close( fd );
				return ETCP_BIND_FAILED;
			}
		}
	}

	//非阻塞
	mysock_noblock(fd, 1);

	//连接
	if(timeout <= 0) 
	{
		timeout = 15000;
	}

	timeval start_timeTemp, end_timeTemp;
	TimeUtil::gettimeofday(&start_timeTemp, NULL);

	int rc = 0;	
	if (ipv6)
	{
		rc = mysock_connect_ipport_v6(fd, serv_ip, serv_port);
	}
	else
	{
		rc = mysock_connect_ipport(fd, serv_ip, serv_port);
	}

	int so_err = 0;
	rc = mysock_wait_connect2(fd, rc, timeout, conn_break, &so_err);
	TimeUtil::gettimeofday(&end_timeTemp, NULL);
	take_ms = TimeUtil::diffTimevalMS(end_timeTemp, start_timeTemp);

	if( rc < 0)
	{
		logger->logFmt("mysock_wait_connect failed rc=%d so_err=%d take=%f", rc, so_err, take_ms);
		mysock_close(fd);

		int conv_err = convert_error(rc, so_err, err_info);
		logger->logFmt("convert_error %d", conv_err);
		return conv_err;
	}

	logger->logFmt("mysock_wait_connect success fd=(%d)", fd);
	return fd;
}

int TcpUtil::tcp_accept(char* local_if, int local_port, int send_buff_size, int recv_buff_size, int timeout, comm::Logger* logger, int* conn_break, char* err_info)
{
	int rc;
	int fd;

	logger->log("TcpUtil::tcp_accept start");

	if( (fd = mysock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1 )
	{
		logger->log("socket failed");
		return ETCP_OTHER;
	}

	socket_buff_set(fd, send_buff_size, recv_buff_size, logger);

	//绑定地址
	if((local_if!=NULL && *local_if) || local_port!=0)
	{
		int addr_reuse = 1;
		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&addr_reuse, sizeof(addr_reuse));

		logger->logFmt("socket_bind local_if:%s port:%d", local_if, local_port);
		if(mysock_bind(fd, local_if, local_port) == -1)
		{
			logger->logFmt("socket error: %s", mysock_error());
			mysock_close( fd );
			return ETCP_BIND_FAILED;
		}
	}

	//非阻塞
	mysock_noblock(fd, 1);

	//每次重连超时(默认为15000ms)
	if(timeout <= 0) 
	{
		timeout = 15000;
	}

	rc = mysock_listen(fd, 5);
	if (rc < 0)
	{
		logger->logFmt("mysock_listen failed ret=%d err=%s", rc, mysock_error());
		mysock_close(fd);
		return ETCP_OTHER;
	}

	//有新的连接。
	logger->logFmt("mysock_accept waiting %d", timeout);

	auto _start_ticks = std::chrono::steady_clock::now();
	int new_fd = mysock_accept(fd, timeout, conn_break);

	if(new_fd < 0)
	{
		int so_err = mysock_option_get_soerror(fd);
		const char* sock_err = mysock_error();
		int _take = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(
	std::chrono::steady_clock::now() - _start_ticks).count());
		logger->logFmt("mysock_accept failed fd=%d so_err=%d sock_err=%s take=%d", new_fd, so_err, sock_err, _take);
		mysock_close(fd);

		int conv_err = convert_error(new_fd, so_err, err_info);
		logger->logFmt("convert_error %d", conv_err);
		return conv_err;
	}

	logger->logFmt("mysock_accept success (fd=%d)", new_fd);
	mysock_close(fd);
	return new_fd;
}

void TcpUtil::socket_buff_set(int fd, int send_buff_size, int recv_buff_size, comm::Logger* logger)
{
	//output_snd_rcv_buf(fd, logger);	

	if (send_buff_size > 0)
	{
		if (mysock_option_send_windowsize(fd, send_buff_size) == -1)
		{
			logger->log("mysock_option_send_windowsize failed");
		}
	}

	if (recv_buff_size > 0)
	{
		if (mysock_option_recv_windowsize(fd, recv_buff_size) == -1)
		{
			logger->log("mysock_option_recv_windowsize failed");
		}
	}	

	if (send_buff_size > 0 || recv_buff_size > 0)
	{
		//logger->log("after buff setting");
		//output_snd_rcv_buf(fd, logger);
	}
}

void TcpUtil::output_snd_rcv_buf(int fd, comm::Logger* logger)
{
	int snd_buf = 0;
	SOCKLEN_T buff_len = sizeof(snd_buf);
#if defined(__ANDROID_ARM64__)
	getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char *)&snd_buf, (socklen_t*)&buff_len);
#else
	getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char *)&snd_buf, &buff_len);
#endif
	

	int rcv_buf = 0;
	buff_len = sizeof(rcv_buf);

#if defined(__ANDROID_ARM64__)
	getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *)&rcv_buf, (socklen_t*)&buff_len);
#else
	getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *)&rcv_buf, &buff_len);
#endif
	

	logger->logFmt("snd_buf:%d rcv_buf:%d", snd_buf, rcv_buf);
}


int TcpUtil::convert_error(int conn_err, int so_err, char* err_info)
{
	if(conn_err==-2)
	{
		strcpy(err_info, "user stop");
		return ETCP_USER_STOP;
	}

#if defined(WIN32)
	else if (so_err==WSAENETUNREACH)
	{
		strcpy(err_info, "network unavailable");
		return ETCP_NET_UNAVAIL;
	}
	else if (so_err==WSAEHOSTUNREACH)
	{
		strcpy(err_info, "host unreach");
		return ETCP_HOST_UNREACH;
	}
	else if (so_err==WSAETIMEDOUT)
	{
		strcpy(err_info, "timeout");
		return ETCP_CONN_TIMEOUT;
	}
	else if (so_err==WSAECONNREFUSED)
	{
		strcpy(err_info, "refused by peer");
		return ETCP_CONN_REFUSED;
	}
#else
	else if (so_err==ENETUNREACH)
	{
		strcpy(err_info, "network unavailable");
		return ETCP_NET_UNAVAIL;
	}
	else if (so_err==EHOSTUNREACH)
	{
		strcpy(err_info, "host unreach");
		return ETCP_HOST_UNREACH;
	}
	else if (so_err==ETIMEDOUT)
	{
		strcpy(err_info, "timeout");
		return ETCP_CONN_TIMEOUT;
	}
	else if (so_err==ECONNREFUSED)
	{
		strcpy(err_info, "refused by peer");
		return ETCP_CONN_REFUSED;
	}
#endif

	else if (conn_err == -4)
	{
		strcpy(err_info, "timeout");
		return ETCP_CONN_TIMEOUT;
	}
	else
	{
		strcpy(err_info, "unknown");
		return ETCP_OTHER;
	}
}
