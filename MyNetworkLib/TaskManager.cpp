#include "pch.h"
#include "TaskManager.h"
#include "TaskManager.h"

NetCore::Thread::TaskManager::TaskManager()
{
	// For main thread.
	InitTLS();
}

NetCore::Thread::TaskManager::~TaskManager()
{
	_join_all_tasks();

	// For main thread
	ClearTLS();

	_tasks.clear();
}

NetCore::task_id NetCore::Thread::TaskManager::AddTask(std::function<void()> task)
{
	_LOCK_GUARD;

	task_id id = _tasks.size() + 1;
	auto th = std::thread([=]() {
		task();

		_task_done(id);
		});
	_tasks.insert(make_pair(id, make_pair(std::move(th), false)));

	return id;
}

void NetCore::Thread::TaskManager::_join_all_tasks()
{
	for (auto& t : _tasks)
	{
		if (t.second.first.joinable()) 
			t.second.first.join();
	}
}

void NetCore::Thread::TaskManager::InitTLS()
{
}

void NetCore::Thread::TaskManager::ClearTLS()
{
}
