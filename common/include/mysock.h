#ifndef MY_SOCK_H__
#define MY_SOCK_H__
#include "common.h"

namespace comm
{

//return -1 if failed. return 0 if ok
int mysock_init();
int mysock_init_with_logger(void* logger);
void mysock_uninit();
bool mysock_reset_logger(void* logger);
int mysock_socket(const int af,const int type,const int protocol);

//socket read and write timeout(be used by unblock socket's select)
void mysock_readtimeout_set(int timeout_ms);
void mysock_writetimeout_set(int timeout_ms);

//return -2:userstop -3:failed -4:timeout. 0:socket closed >0:ok
int mysock_writen(const int fd, unsigned char* indata, int indata_len, int *conn_break);
int mysock_read(const int fd, unsigned char* outdata, int want_datalen, int *conn_break);
int mysock_readn(const int fd, unsigned char* outdata, int want_datalen, int *conn_break);
//read_socket: 1:read 0:write
//return -2:userstop -3:failed -4:timeout. >0:ok. (never be 0)
int mysock_select(const int max_fd, int read_socket, const int timeout_ms, int *break_flag);

//win32: local_if->ip linux: local_if->pppX
int mysock_bind(const int fd, const char* local_if, int local_port);

int mysock_error_code();
void mysock_set_errcode(int err);
void mysock_clear_error();
const char* mysock_error();
int mysock_noblock_error(int err);

void mysock_close(const int fd);

char* mysock_gethostbyname(const char* name, const char* local_if); //not use this, not safe-thread
char* mysock_gethostbyname_aaaa(const char* name, const char* local_if); //not use this, not safe-thread
std::string mysock_gethostbyname_str(const char* name, const char* local_if);  //use this
std::string mysock_gethostbyname_aaaa_str(const char* name, const char* local_if); //use this

int mysock_getsockname(const int fd, char **local_if, int *local_port);
int mysock_noblock(const int fd, int noblock);
int mysock_option_nodelay(const int fd);
int mysock_option_send_windowsize(const int fd, int win_size);
int mysock_option_recv_windowsize(const int fd, int win_size);
int mysock_option_reuse(const int fd);
int mysock_option_get_soerror(const int fd);
int mysock_listen(const int fd, int queSize);
//return handle:success <0:error
int mysock_accept(const int fd, int timeout_ms, int* conn_break);
int mysock_connect_ipport(const int fd, const char* serv_ip, int serv_port);
int mysock_connect_ipport_v6(const int fd, const char* serv_ip, int serv_port);

//return 0:success -1:normal -2:userstop -3:select failed -4:timeout
int mysock_wait_connect(const int fd, int r, int timeout_ms, int* conn_break);
int mysock_wait_connect2(const int fd, int r, int timeout_ms, int* conn_break, int* so_err);

//return port in[portMin, portMax]: success -1 if failed
int mysock_can_useport(const char* ip, int portMin, int portMax);
char* mysock_if2ip(const char *local_if);
std::string mysock_if2ipv6(const char *local_if);
void mysock_release_mutex();

}
#endif

