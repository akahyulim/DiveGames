#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <functional>
#include <memory>

namespace Dive
{
	class Task
	{
	public:
		typedef std::function<void()> functionType;

		Task(functionType&& function)	{ m_function = std::forward<functionType>(function); }
		void Execute()					{ m_bExecuting = true; m_function(); m_bExecuting = false; }
		bool IsExecuting() const		{ return m_bExecuting; }

	private:
		functionType m_function;
		bool m_bExecuting = false;
	};

	class ThreadPool
	{
	public:
		static ThreadPool& GetInstance()
		{
			static ThreadPool threadPool;
			return threadPool;
		}

		void Initialize();

		template<class Function>
		void AddTask(Function&& function)
		{
			if (m_workers.empty())
			{

				function();
				return;
			}

			std::unique_lock<std::mutex> lock(m_mutex);
			// bind를 왜 사용했을까?
			m_tasks.push(std::make_shared<Task>(std::bind(std::forward<Function>(function))));
			lock.unlock();

			m_wakeCondition.notify_one();
		}

		unsigned int GetWorkerCount() const { return m_workerCount; }

	private:
		ThreadPool() = default;
		~ThreadPool();

		void threadLoop();

	private:
		unsigned int m_workerCount = 0;
		std::vector<std::thread> m_workers;
		std::queue<std::shared_ptr<Task>> m_tasks;
		std::mutex m_mutex;
		std::condition_variable m_wakeCondition;

		bool m_bStopAll = false;
	};
}