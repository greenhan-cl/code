#ifndef COMMON_EXT_H__
#define COMMON_EXT_H__

#include "tcputil.h"
#include "iputil.h"
#include "devicehelper.h"
#include "mysock_define.h"
#include "mysock.h"
#include "threadworker.h"
#include "mytimer.h"
#include "oswsharem.h"
#include "cqueue_fix.h"
#include "soloader.h"
#include "dnssoloader.h"
#include "base64.h"
#if defined(__IPHONEOS__)
    #include "dnsutil.h"
#endif

//should mysock_init() first, to initialize socket

#endif //COMMON_EXT_H__
