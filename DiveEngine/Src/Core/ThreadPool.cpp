#include "ThreadPool.h"

using namespace std;

namespace DiveEngine
{
	ThreadPool::~ThreadPool()
	{
		unique_lock<mutex> lock(m_mutex);
		m_bStopAll = true;
		lock.unlock();

		m_wakeCondition.notify_all();

		for (auto& worker : m_workers)
		{
			worker.join();
		}

		m_workers.clear();
	}

	void ThreadPool::Initialize()
	{
		auto numCores = thread::hardware_concurrency();
		m_workerCount = max(1u, numCores - 1);
		
		m_workers.reserve(m_workerCount);
		for (unsigned int i = 0; i < m_workerCount; i++)
		{
			m_workers.emplace_back([this]() {this->threadLoop(); });
		}

		CORE_TRACE("{:d}개의 Worker Thread가 생성되었습니다.", m_workerCount);
	}

	void ThreadPool::threadLoop()
	{
		shared_ptr<Task> task;
		while (true)
		{
			unique_lock<mutex> lock(m_mutex);

			m_wakeCondition.wait(lock, [this] {return !m_tasks.empty() || m_bStopAll; });

			if (m_bStopAll && m_tasks.empty())
				return;

			task = m_tasks.front();
			m_tasks.pop();

			lock.unlock();

			task->Execute();
		}
	}
}