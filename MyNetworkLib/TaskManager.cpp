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
		InitTLS();
		task();

		ClearTLS();
		_task_done(id);
		});
	_tasks.insert(make_pair(id, make_pair(std::move(th), false)));

	return id;
}

pair<NetCore::task_id, NetCore::task_id> NetCore::Thread::TaskManager::AddTask(std::function<void()> task, const count_t count)
{
	_LOCK_GUARD;

	const task_id first = _tasks.size() + 1;

	for (auto i = 0; i < count; ++i)
	{
		task_id id = _tasks.size() + 1;
		auto th = std::thread([=]() {
			InitTLS();
			task();

			ClearTLS();
			_task_done(id);
			});
		_tasks.insert(make_pair(id, make_pair(std::move(th), false)));
	}

	const task_id second = _tasks.size();

	return { first, second };
}

void NetCore::Thread::TaskManager::_join_all_tasks()
{
	for (auto& t : _tasks)
	{
		if (t.second.first.joinable()) 
			t.second.first.join();
	}
}

bool NetCore::Thread::TaskManager::JoinTask(const task_id id)
{
	_LOCK_GUARD;

	if (_tasks.at(id).second == true)
	{
		if (_tasks.at(id).first.joinable())
		{
			_tasks.at(id).first.join();
			return true;
		}
	}

	return false;
}

void NetCore::Thread::TaskManager::InitTLS()
{
}

void NetCore::Thread::TaskManager::ClearTLS()
{
}
