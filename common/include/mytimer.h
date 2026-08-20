#ifndef MYTIMER_H__
#define MYTIMER_H__

#include <chrono>
#include <thread>

#include "platinc.h"
#include "threadworker.h"

namespace comm
{
class MyTimerListener {
public:
	virtual void onTimer() = 0;
};

class MyTimer : public comm::ThreadWorker
{
public:
	MyTimer(MyTimerListener* listener, unsigned int interval_ms, comm::Logger* logger)
		:ThreadWorker("MyTimer", logger, true, false)
	{
		m_listener = listener;
		m_interval_ms = interval_ms;
		m_last_ticks = 0;
	}

protected:
	virtual void do_start(){
		m_last_ticks = static_cast<UINT64>(std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now().time_since_epoch()).count());
	}

	virtual void do_stop(){
	}

	virtual bool do_work() 
	{
		UINT64 now_tick = static_cast<UINT64>(std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now().time_since_epoch()).count());
		if (now_tick - m_last_ticks >= m_interval_ms)
		{
			m_listener->onTimer();
			m_last_ticks = now_tick;
		}

		if (m_interval_ms >=1000)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		return true; //continue
	}

protected:
	MyTimerListener* m_listener;
	unsigned int m_interval_ms;
	UINT64 m_last_ticks;
};
}

#endif


int test(int arr[][10], int num) {
	return -1;
}

void aaa()
{
	int arr[10][10];
	test(arr, 10);
	return ;
}
