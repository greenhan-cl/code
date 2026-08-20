#ifndef TCP_UTIL_H__
#define TCP_UTIL_H__

#include "logger.h"

namespace comm
{

#define ETCP_NET_UNAVAIL	-1
#define ETCP_HOST_UNREACH	-2
#define ETCP_CONN_TIMEOUT	-3
#define ETCP_CONN_REFUSED	-4
#define ETCP_CONN_BREAK		-5
#define ETCP_BIND_FAILED	-6
#define ETCP_USER_STOP		-12
#define ETCP_OTHER			-15

class TcpUtil
{
public:
	static int tcp_connect(const char *serv_ip, int serv_port, const char* local_if, int send_buff_size, int recv_buff_size, int timeout, comm::Logger* logger, int* conn_break, char* err_info);
	static int tcp_connect_take(const char *serv_ip, int serv_port, const char* local_if, int send_buff_size, int recv_buff_size, int timeout, comm::Logger* logger, int* conn_break, char* err_info, double& take_ms);

	static int tcp_accept(char* local_if, int local_port, int send_buff_size, int recv_buff_size, int timeout, comm::Logger* logger, int* conn_break, char* err_info);
	static void socket_buff_set(int fd, int send_buff_size, int recv_buff_size, comm::Logger* logger);
	static void output_snd_rcv_buf(int fd, comm::Logger* logger);
	static int convert_error(int conn_err, int so_err, char* err_info);
};
}
#endif
