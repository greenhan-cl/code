#ifndef MYSOCK_DEFINE_H__
#define MYSOCK_DEFINE_H__

#include <stdio.h>
#include <string.h>
#include <map>
#include <string>

#if defined(WIN32) || defined(_WIN32_WCE)
#include <WinSock2.h>
#include <Windows.h>
#else
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <net/if.h>
#include <sys/ioctl.h>
#endif

#if defined(_WIN32) || defined(ANDROID)
typedef int SOCKLEN_T;
#else
typedef unsigned int SOCKLEN_T;
#endif

#endif //MYSOCK_DEFINE_H__
