#include "timeutil.h"
#include <time.h>
#include <string.h>

#if defined(WIN32) && !defined(_WIN32_WCE)
#include <sys/timeb.h>

#else
#include <sys/types.h>
#include <sys/time.h>

#if defined(ANDROID)
	struct timeb
	{
		time_t time;
		unsigned short millitm;
		short timezone;
		short dstflag;
	}; 
#else
#include <sys/timeb.h>
#endif

#include <stdlib.h>

#ifndef HAS_FTIME
int ftime (struct timeb *timebuf)
{
  struct timeval tv;
  struct timezone tz;

  if (gettimeofday (&tv, &tz) < 0)
    return -1;

  timebuf->time = tv.tv_sec;
  timebuf->millitm = (tv.tv_usec + 500) / 1000;
  if (timebuf->millitm == 1000)
    {
      ++timebuf->time;
      timebuf->millitm = 0;
    }
  timebuf->timezone = tz.tz_minuteswest;
  timebuf->dstflag = tz.tz_dsttime;
  return 0;
};
#endif

#endif



using namespace std;
using namespace comm;

double TimeUtil::diffTimevalMS(timeval t1, timeval t2)
{
	return (t1.tv_sec - t2.tv_sec) * 1000.0 + (t1.tv_usec - t2.tv_usec) / 1000.0;
}

UINT64 TimeUtil::convertTimevalToMS(const timeval &tv) {
	return ((UINT64)tv.tv_sec) * 1000 + (tv.tv_usec / 1000);
}

timeval TimeUtil::convertMSToTimeval(UINT64 ms)
{
	timeval tv;
	tv.tv_sec = (long)(ms / 1000);
	tv.tv_usec = (long)((ms % 1000) * 1000);
	return tv;
}

static tm* my_localtime(time_t* t)
{
#if defined(GMTTIME_TO_LOCALTIME)
	static struct tm china_time;
	struct tm utc_time;
	gmtime_r(t, &utc_time);

	china_time = utc_time;
	china_time.tm_hour += 8;
	return &china_time;

#else
	return localtime(t);
#endif
}

tm TimeUtil::gettimeofday_tm()
{
	struct tm *local;
	time_t t;
	t = time(NULL);
	local=my_localtime(&t);
	return *local;
}

time_t TimeUtil::gettimeofday_timet(){
	return time(NULL);
}

UINT64 TimeUtil::gettimeofday_timestamp()
{
	timeval tv;
	TimeUtil::gettimeofday(&tv, NULL);
	return TimeUtil::convertTimevalToMS(tv);
}

//20160628_123021
std::string TimeUtil::getymd_hns()
{
	char ymd_nhs[100];
	memset(ymd_nhs, 0x00, sizeof(ymd_nhs));

	struct tm *local;
	time_t t;
	t=time(NULL);
	local=my_localtime(&t);

	sprintf(ymd_nhs, "%04d%02d%02d_%02d%02d%02d",
		1900+local->tm_year, 1+local->tm_mon,
		local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);

	return string(ymd_nhs);
}

//20160628123021
std::string TimeUtil::getymdhns()
{
	char ymd_nhs[100];
	memset(ymd_nhs, 0x00, sizeof(ymd_nhs));

	struct tm *local;
	time_t t;
	t=time(NULL);
	local=my_localtime(&t);

	sprintf(ymd_nhs, "%04d%02d%02d%02d%02d%02d",
		1900+local->tm_year, 1+local->tm_mon,
		local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);

	return string(ymd_nhs);
}

//2016/06/28 12:30:21
std::string TimeUtil::getymd_hns_std()
{
	char ymd_nhs[100];
	memset(ymd_nhs, 0x00, sizeof(ymd_nhs));

	struct tm *local;
	time_t t;
	t=time(NULL);
	local=my_localtime(&t);

	sprintf(ymd_nhs, "%04d/%02d/%02d %02d:%02d:%02d",
		1900+local->tm_year, 1+local->tm_mon,
		local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);

	return string(ymd_nhs);
}

std::string TimeUtil::getymd_hns_std2()
{
	char ymd_nhs[100];
	memset(ymd_nhs, 0x00, sizeof(ymd_nhs));

	struct tm *local;
	time_t t;
	t=time(NULL);
	local=my_localtime(&t);

	sprintf(ymd_nhs, "%04d-%02d-%02d %02d:%02d:%02d",
		1900+local->tm_year, 1+local->tm_mon,
		local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);

	return string(ymd_nhs);
}

//2016/06/28 12:30:21
std::string TimeUtil::getymd_format_server(const char* fmt, int servTimeDiff)
{
	char ymd_nhs[100];
	memset(ymd_nhs, 0x00, sizeof(ymd_nhs));

	struct tm *local;
	time_t t;
	t = time(NULL);
	t += servTimeDiff;
	local=my_localtime(&t);

	sprintf(ymd_nhs, fmt,
		1900+local->tm_year, 1+local->tm_mon,
		local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);

	return string(ymd_nhs);
}

std::string TimeUtil::getymd_format_server_timet(const char* fmt, time_t t1, int servTimeDiff)
{
	char ymd_nhs[100];
	memset(ymd_nhs, 0x00, sizeof(ymd_nhs));

	struct tm *local;
	time_t t;
	t = t1;
	t += servTimeDiff;
	local=my_localtime(&t);

	sprintf(ymd_nhs, fmt,
		1900+local->tm_year, 1+local->tm_mon,
		local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);

	return string(ymd_nhs);
}

std::string TimeUtil::getymd_format(const char* fmt)
{
	char szHnsm[100];
	memset(szHnsm, 0x00, sizeof(szHnsm));

	struct tm *local;
	struct timeb tp;

	ftime(&tp);
	local=my_localtime(&(tp.time));

	sprintf(szHnsm, fmt,
		1900+local->tm_year, 1+local->tm_mon,
		local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec, tp.millitm);

	return string(szHnsm);
}

//20160628_123021_021
std::string TimeUtil::getymd_hnsm()
{
	const char* time_fmt = "%04d%02d%02d_%02d%02d%02d_%03d";

	char szHnsm[100];
	memset(szHnsm, 0x00, sizeof(szHnsm));

	struct tm *local;
	struct timeb tp;

	ftime(&tp);
	local=my_localtime(&(tp.time));

	sprintf(szHnsm, time_fmt,
		1900+local->tm_year, 1+local->tm_mon,
		local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec, tp.millitm);

	return string(szHnsm);
}

//2016/06/28 12:30:21.021
std::string TimeUtil::getymd_hnsm_std()
{
	const char* time_fmt = "%04d/%02d/%02d %02d:%02d:%02d.%03d";

	char szHnsm[100];
	memset(szHnsm, 0x00, sizeof(szHnsm));

	struct tm *local;
	struct timeb tp;

	ftime(&tp);
	local=my_localtime(&(tp.time));

	sprintf(szHnsm, time_fmt,
		1900+local->tm_year, 1+local->tm_mon,
		local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec, tp.millitm);

	return string(szHnsm);
}


//12:30:21.021
std::string TimeUtil::gethnsm_std()
{
	const char* time_fmt = "%02d:%02d:%02d.%03d";

	char szHnsm[100];
	memset(szHnsm, 0x00, sizeof(szHnsm));

	struct tm *local;
	struct timeb tp;

	ftime(&tp);
	local=my_localtime(&(tp.time));

	sprintf(szHnsm, time_fmt,
		local->tm_hour, local->tm_min, local->tm_sec, tp.millitm);

	return string(szHnsm);
}

int TimeUtil::gettimeofday(struct timeval* tp, int* tz)
{
#if defined(_WIN32)
	static LARGE_INTEGER tickFrequency, epochOffset;

	// For our first call, use "ftime()", so that we get a time with a proper epoch.
	// For subsequent calls, use "QueryPerformanceCount()", because it's more fine-grain.
	static bool isFirstCall = true;

	LARGE_INTEGER tickNow;
	QueryPerformanceCounter(&tickNow);

	if (isFirstCall) 
	{
		struct timeb tb;
		ftime(&tb);
		tp->tv_sec = (long)tb.time;
		tp->tv_usec = 1000 * tb.millitm;

		// Also get our counter frequency:
		QueryPerformanceFrequency(&tickFrequency);

		// And compute an offset to add to subsequent counter times, so we get a proper epoch:
		epochOffset.QuadPart
			= tb.time*tickFrequency.QuadPart + (tb.millitm*tickFrequency.QuadPart)/1000 - tickNow.QuadPart;

		isFirstCall = false; // for next time
	}
	else
	{
		// Adjust our counter time so that we get a proper epoch:
		tickNow.QuadPart += epochOffset.QuadPart;

		tp->tv_sec = (long) (tickNow.QuadPart / tickFrequency.QuadPart);
		tp->tv_usec = (long) (((tickNow.QuadPart % tickFrequency.QuadPart) * 1000000L) / tickFrequency.QuadPart);
	}
#else
	return ::gettimeofday(tp, NULL);
#endif
	return 0;
}

std::string TimeUtil::format_timet(const char* fmt, time_t time1)
{
	char szTime[100];
	struct tm tm1;  

#ifdef _WIN32  
	tm1 = *my_localtime(&time1);  
#else  
	localtime_r(&time1, &tm1);  
#endif  
	sprintf( szTime, fmt,  
		tm1.tm_year+1900, tm1.tm_mon+1, tm1.tm_mday,  
		tm1.tm_hour, tm1.tm_min,tm1.tm_sec);
	return szTime;
}

std::string TimeUtil::format_timestamp(const char* fmt, UINT64 timestamp)
{
	char szTime[100];
	struct tm tm1;  

	time_t time1 = (time_t)(timestamp/1000);
	int ms = timestamp % 1000;

#ifdef _WIN32  
	tm1 = *my_localtime(&time1);  
#else  
	localtime_r(&time1, &tm1);  
#endif  
	sprintf( szTime, fmt,  
		tm1.tm_year+1900, tm1.tm_mon+1, tm1.tm_mday,  
		tm1.tm_hour, tm1.tm_min,tm1.tm_sec, ms);
	return szTime;
}

time_t TimeUtil::Str2Timet(const char* fmt, const char* szTime)
{
	struct tm tm1;  
	time_t time1; 

	sscanf(szTime, fmt,      
		&tm1.tm_year,   
		&tm1.tm_mon,   
		&tm1.tm_mday,   
		&tm1.tm_hour,   
		&tm1.tm_min,  
		&tm1.tm_sec);  

	tm1.tm_year -= 1900;  
	tm1.tm_mon --;  

	tm1.tm_isdst=-1;  

	time1 = mktime(&tm1);  
	return time1;
}
