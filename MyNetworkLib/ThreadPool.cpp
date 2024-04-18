#include "pch.h"
#include "ThreadPool.h"

NetCore::Thread::ThreadPool::ThreadPool(const uint32 maxPoolCount)
	:MAX_POOL_COUNT(maxPoolCount)
{
	for (uint32 i = 0; i < MAX_POOL_COUNT; ++i)
	{
		_workers.emplace_back([this]
			{
				while (true)
				{
					std::function<void()> task;
					{
						std::unique_lock<Mutex> lock(this->_mutex);
						this->_condition.wait(lock, [this] { return this->_stop || !this->_waiting_queue.empty(); });
						if (this->_stop && this->_waiting_queue.empty()) return;
						task = std::move(this->_waiting_queue.front());
						this->_waiting_queue.pop();
					}
					task();
				}
			});
	}
}

NetCore::Thread::ThreadPool::~ThreadPool()
{
	DESTRUCTOR(ThreadPool);

	{
		std::unique_lock<Mutex> lock(_mutex);
	}
	_condition.notify_all();

	for (auto& thread : _workers)
	{
		if (thread.joinable()) thread.join();
	}
}

