#include "threadworker.h"
#include <chrono>
#include <sstream>
#include <system_error>
#include <thread>
using namespace std;
using namespace comm;

ThreadWorker::ThreadWorker(const std::string& tag, comm::Logger* logger, 
						   bool shoudWaitIfCancel, bool shouldInnerDelay)
{
	m_tag = tag;
	m_logger = logger;
	m_shoudWaitIfCancel = shoudWaitIfCancel;
	m_innerDelay = shouldInnerDelay;

	m_cancel.store(false);
	m_running.store(false);
}

ThreadWorker::~ThreadWorker()
{
	if (m_worker.joinable())
	{
		if(m_logger != nullptr)
			m_logger->log(m_tag + " wait_thread");
		m_worker.join();
	}
}

void ThreadWorker::start(){
	if(m_logger != nullptr)
		m_logger->log(m_tag + " start");
	if (m_worker.joinable())
	{
		if (m_logger != nullptr)
			m_logger->log(m_tag + " already started");
		return;
	}

	m_cancel.store(false);
	do_start();
	try
	{
		m_worker = std::thread(&ThreadWorker::run, this);
	}
	catch (const std::system_error& _error)
	{
		if (m_logger != nullptr)
			m_logger->log(m_tag + " create thread failed: " + _error.what());
	}
}

void ThreadWorker::stop()
{
	if(m_logger != nullptr)
		m_logger->log(m_tag + " stop");
	m_cancel.store(true);
	do_stop();

	if (m_worker.joinable())
	{
		if(m_logger != nullptr)
			m_logger->log(m_tag + " wait_thread");
		m_worker.join();
	}

	if(m_logger != nullptr)
		m_logger->log(m_tag + " stopped");
}

void ThreadWorker::run(ThreadWorker* worker)
{
	worker->m_running.store(true);
	if(worker->m_logger != nullptr){
		std::ostringstream _thread_id;
		_thread_id << std::this_thread::get_id();
		worker->m_logger->log(worker->m_tag + string(" ThreadWorker thread...threadid=")
			+ _thread_id.str());
	}

	while (true)
	{	
		if(worker->m_cancel.load()){
			if(worker->m_logger != nullptr)
				worker->m_logger->log(worker->m_tag + " cancel break");
			break;
		}

		if(!worker->do_work())
		{
			break;
		}

		if (worker->m_innerDelay)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}

	if(worker->m_logger != nullptr)
		worker->m_logger->log(worker->m_tag + " ThreadWorker thread exit");
	worker->m_running.store(false);
}
