#ifndef BAND_LIMITER
#define BAND_LIMITER

#include <chrono>
#include <thread>

#include "logger.h"
#include "macroutil.h"
#include "platinc.h"

using namespace comm;

class BandLimiter
{
public:
	BandLimiter(int range_ms, int bytePerSecond, int* stopFlagIntPtr, comm::Logger* logger)
	{
		m_range_ms = range_ms;
		m_bytePerSecond = bytePerSecond;
		m_stopFlagIntPtr = stopFlagIntPtr;
		m_logger = logger;

		m_tmpRangeShouldRecvBytes = bytePerSecond /1000 * m_range_ms;
		m_rangeShouldRecvBytes = m_tmpRangeShouldRecvBytes;
		m_thisRangeRecvBytes = 0;
		m_thisRangeStartTicket = 0;
		m_firstComData = true;
		m_resetRecvTicket = 0;
		m_logger->logFmt("m_range_ms:%d m_bytePerSecond=%d m_rangeShouldRecvBytes=%d", 
			m_range_ms, m_bytePerSecond, m_rangeShouldRecvBytes);
	}

	void comeData(int recv_size)
	{
		if (m_bytePerSecond <= 0)
			return;

		UINT64 now_tick = static_cast<UINT64>(std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now().time_since_epoch()).count());
		if (m_firstComData)
		{
			m_firstComData = false;
			m_thisRangeStartTicket = now_tick;
			//m_logger->logFmt("entry first range.%d recv=%d", m_thisRangeStartTicket, recv_size);
		}

		if (now_tick - m_thisRangeStartTicket >= m_range_ms)
		{
			m_thisRangeStartTicket += m_range_ms;
			m_thisRangeRecvBytes = recv_size;
			
			if (m_thisRangeStartTicket-m_resetRecvTicket >= 1000)
			{
				m_resetRecvTicket = m_thisRangeStartTicket;
				m_rangeShouldRecvBytes = m_tmpRangeShouldRecvBytes;
			}

			//m_logger->logFmt("come to next range.%d recv=%d", m_thisRangeStartTicket, m_thisRangeRecvBytes);
		}
		else
		{
			m_thisRangeRecvBytes += recv_size;
			//m_logger->logFmt("this range %d recv %d", m_thisRangeStartTicket, m_thisRangeRecvBytes);

			if (m_thisRangeRecvBytes >= m_rangeShouldRecvBytes)
			{
#ifdef _WIN32
				int should_delay = (int)(m_thisRangeStartTicket + m_range_ms - now_tick) + 10;
#else
				int should_delay = (int)(m_thisRangeStartTicket + m_range_ms - now_tick);
#endif
				
				m_rangeShouldRecvBytes =  m_rangeShouldRecvBytes - (m_thisRangeRecvBytes - m_tmpRangeShouldRecvBytes);
				//m_logger->logFmt("this range has recv full. %d now idle %d. next adjust %d.", m_thisRangeRecvBytes, should_delay, m_rangeShouldRecvBytes);

				if (should_delay > 0)
				{
					if (m_stopFlagIntPtr!=NULL)
					{
						DT_DELAY_COND(should_delay, !(*m_stopFlagIntPtr));
					}
					else
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(should_delay));
					}
				}
			}
			else
			{

			}
		}
	}

private:
	int							m_range_ms;
	int							m_bytePerSecond;
	int							*m_stopFlagIntPtr;
	comm::Logger*				m_logger;

	int							m_thisRangeRecvBytes;
	UINT64				m_thisRangeStartTicket;
	int							m_rangeShouldRecvBytes;
	int							m_tmpRangeShouldRecvBytes;
	bool						m_firstComData;
	UINT64				m_resetRecvTicket;
};

#endif
