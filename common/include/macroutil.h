#ifndef MACRO_UTIL_H__
#define MACRO_UTIL_H__

#include <chrono>
#include <thread>

namespace comm
{

//route becarefull use this
#define DT_DELAY_COND(delay_ms, runFlag) \
{ \
	if (runFlag) \
	{ \
		auto _start_t = std::chrono::steady_clock::now(); \
		while(true) \
		{ \
			auto _now_t = std::chrono::steady_clock::now(); \
			if (!(runFlag) || std::chrono::duration_cast<std::chrono::milliseconds>(_now_t - _start_t).count() >= (delay_ms)) \
			{ \
				break; \
			} \
			std::this_thread::sleep_for(std::chrono::milliseconds(10)); \
		} \
	} \
} \

#define DT_DELAY_COND3(delay_ms, runFlag, minDelay) \
{ \
	if (runFlag) \
	{ \
		auto _start_t = std::chrono::steady_clock::now(); \
		while(true) \
		{ \
			auto _now_t = std::chrono::steady_clock::now(); \
			if (!(runFlag) || std::chrono::duration_cast<std::chrono::milliseconds>(_now_t - _start_t).count() >= (delay_ms)) \
			{ \
				break; \
			} \
			std::this_thread::sleep_for(std::chrono::milliseconds(minDelay)); \
		} \
	} \
} \

#define SAFE_DELETE(obj) \
{ \
	if(obj != NULL) \
	{ \
		delete obj; \
		obj = NULL; \
	} \
} \

} //namespace comm

#endif

