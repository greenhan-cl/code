#ifndef THREAD_WORKER_H__
#define THREAD_WORKER_H__

#include <atomic>
#include <string>
#include <thread>

#include "logger.h"

namespace comm
{

class ThreadWorker {
	std::string m_tag;
	bool m_shoudWaitIfCancel;
	std::thread m_worker;
	bool m_innerDelay;

protected:
	std::atomic<bool> m_cancel;
	std::atomic<bool> m_running;
	comm::Logger* m_logger;

public:
	ThreadWorker(const std::string& tag, comm::Logger* logger, 
				bool shoudWaitIfCancel=true, bool shouldInnerDelay=true);
	virtual ~ThreadWorker();

	void start();
	void stop();
	std::thread* getWorker() { return m_worker.joinable() ? &m_worker : nullptr; }

protected:
	virtual void do_start(){
	}
	virtual void do_stop(){
	}

	virtual bool do_work() = 0;

private:
	static void run(ThreadWorker* worker);
};
}
#endif
