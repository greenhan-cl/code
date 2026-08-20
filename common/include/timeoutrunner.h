#ifndef TIMEOUTWORKER_H__
#define TIMEOUTWORKER_H__

#include <chrono>

#include "platinc.h"
#include "processhelper.h"
#include "threadworker.h"

class TimeoutRunner : public comm::ThreadWorker
{
public:
	TimeoutRunner( int timeout_ms, const std::string& cmd, comm::Logger* logger)
		:ThreadWorker("TimeoutWorker", NULL)
	{
		m_timeout_ms = timeout_ms;
		m_cmd = cmd;
		m_hasTimeout = false;
		m_start_ticks = 0;
	}

	bool hasTimeout(){
		return m_hasTimeout;
	}

protected:
	virtual void do_start(){
		m_start_ticks = static_cast<UINT64>(std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now().time_since_epoch()).count());
	}

	virtual void do_stop(){
	}

	virtual bool do_work() 
	{
		UINT64 now_tick = static_cast<UINT64>(std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now().time_since_epoch()).count());
		if (now_tick - m_start_ticks > m_timeout_ms)
		{
			if (m_logger != NULL)
			{
				m_logger->logFmt("time arrived. %d ms", m_timeout_ms);
			}

			m_hasTimeout = true;
			if (m_cmd != "")
			{
				comm::ProcessHelper helper(m_logger);
				helper.system_run(m_cmd);
			}

			return false;
		}

		return true; //continue
	}

protected:
	int m_timeout_ms;
	std::string m_cmd;
	UINT64 m_start_ticks;
	bool m_hasTimeout;
};

#endif
