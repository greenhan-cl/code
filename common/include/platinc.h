#ifndef PLAT_INC_H__
#define PLAT_INC_H__

#if defined(_WIN32) || defined(_WIN32_WCE)
	#include <WinSock2.h>
	#include <Windows.h>

	#define csnprintf _snprintf
	#define cvsnprintf _vsnprintf
	// MSVC 2015 and later provide standard snprintf. Redefining it is
	// rejected by the Universal CRT, so keep this alias only for old MSVC.
	#if defined(_MSC_VER) && _MSC_VER < 1900
		#define snprintf _snprintf
	#endif

	#define INT64OUTFORMAT "%I64d"
	#define UINT64OUTFORMAT "%I64u"
#else
	#include <sys/socket.h>
	#include <sys/select.h>
	#include <sys/time.h>

#if !defined(ANDROID)
	#include <sys/timeb.h>
#endif

	#include <sys/types.h>
	#include <sys/wait.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <errno.h>
    #include <net/if.h>
    #include <string.h>
	#include <stdio.h>

	#define csnprintf snprintf
	#define cvsnprintf vsnprintf
	typedef long long INT64;
	typedef unsigned long long	UINT64;
	typedef unsigned char BYTE;
	typedef unsigned long  DWORD;
	#define INT64OUTFORMAT "%lld"
	#define UINT64OUTFORMAT "%llu"
#endif

#if defined(ANDROID) //ANDROID Makefile中定义__LINUX__无效
	#define __LINUX__	1
#endif

// Platform-independent defaults.
#define HEAD_CMD "head"
#define LANIP_ETH "br-lan"
#define NAMESERVER_TAG "nameserver"
#define BUYED_DEVICE false
#define DNS_FILENAME "/tmp/resolv.conf.auto"
#define CMD_NOHUP ""
#define DEFAULT_LOGPRINT true
#define PRINT_SELFUPDATELOG

#define DEFAULT_CANSELF_UPDATE	true
#define FINDIP_USETRACE	true
#define CAN_CMDREQUEST	true
#define CAN_V6ENVCHECK	true

#define DEFAULT_ENVCHECK_NETMAXDOWN (128 * 1024)
#define DEFAULT_ENVCHECK_NETMAXUP (128 * 1024)
#define DEFAULT_ENVCHECK_MANYMACHINE	0
#define DEFAULT_ENVCHECK_CPUMAXUSE 15
#define DEFAULT_ENVCHECK_MEMMINFREEKB 8000

#define IPC_SERVER_PORT 8201
#define MAX_WAN_COUNT		4
#define CALC_EVERY_WAN			false

#if defined(ANDROID)
#define PLAT_MODEL "Android"
#define PLAT_EXTRA "Android"

#elif defined(__LINUX__)
#define PLAT_SUB	"LS"
#define PLAT_PRE	"LSIMEI"
#define PLAT_MODEL "LinuxServer"
#define  CUR_TEST_TYPE "TaierLS"
#define DLSPEED_NAME "dlspeed_ls"

#undef DNS_FILENAME
#define DNS_FILENAME "/etc/resolv.conf"

#undef CMD_NOHUP 
#define CMD_NOHUP "nohup"

#if defined(__aarch64__)
#define PLAT_EXTRA "aarch64"
#elif defined(__x86_64__)
#define PLAT_EXTRA "x86_64"
#elif defined(__i386__)
#define PLAT_EXTRA "i686"
#else
#define PLAT_EXTRA "unknown"
#endif

#define HISTORY_SAVE_INBASE 1

#elif defined(_WIN32)
#define PLAT_SUB	"WIN"
#define PLAT_PRE	"WINIMEI"
#define PLAT_MODEL "Windows"
#define CUR_TEST_TYPE "TaierWin"
#define DLSPEED_NAME "dlspeed_ls.exe"
#define PLAT_EXTRA "win32"
#define CMD_NOHUP ""

#define HISTORY_SAVE_INBASE 1
//#define USE_HARDWARE_SPEED	1  //for test

#undef FINDIP_USETRACE	
#define FINDIP_USETRACE	false

#endif

#endif
