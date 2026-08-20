#ifndef TIME_UTIL_H__
#define TIME_UTIL_H__

#include "platinc.h"
#include <string>

namespace comm
{
/**
* 时间相关函数 
*/
class TimeUtil
{
public:
	static double diffTimevalMS(timeval t1, timeval t2);
	//UINT64 Ms(s*1000+us/1000)
	//timeval (s, us)
	static UINT64 convertTimevalToMS(const timeval &tv);
	static timeval convertMSToTimeval(UINT64 ms);

	//result in return(tm，精度s)
	static tm gettimeofday_tm();
	//result in return(time_t，精度s)
	static time_t gettimeofday_timet();
	//result in tp(timeval，精度us)
	static int gettimeofday(struct timeval* tp, int* tz);
	//result in return(ms，精度us)
	static UINT64 gettimeofday_timestamp();

	//20160628_123021
	static std::string getymd_hns();
	//20160628123021
	static std::string getymdhns();
	//20160628_123021_021
	static std::string getymd_hnsm();
	//as:%04d/%02d/%02d %02d:%02d:%02d
	static std::string getymd_format(const char* fmt);
	static std::string getymd_format_server(const char* fmt, int servTimeDiff);
	static std::string getymd_format_server_timet(const char* fmt, time_t t1, int servTimeDiff);

	//2016/06/28 12:30:21
	static std::string getymd_hns_std();
	//2016-06-28 12:30:21
	static std::string getymd_hns_std2();

	//2016/06/28 12:30:21.021
	static std::string getymd_hnsm_std();
	//12:30:21.021
	static std::string gethnsm_std();

	//fmt: as %04d-%02d-%02d %02d:%02d:%02d
	static std::string format_timet(const char* fmt, time_t time1);
	//fmt: as %04d-%02d-%02d %02d:%02d:%02d.%03d
	static std::string format_timestamp(const char* fmt, UINT64 timestamp);
	static time_t Str2Timet(const char* fmt, const char* szTime);
};

}

#endif
