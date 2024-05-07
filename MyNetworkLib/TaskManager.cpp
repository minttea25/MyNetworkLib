#include "pch.h"
#include "TaskManager.h"
#include "SendBufferManager.h"

// Note: TLS_Id must not be 0, because of RWLock flag (0 means empty flag).
// Task Id (=TLS_Id) starts at 1. (Main thread has TLS_Id 1.)
NetCore::Atomic<NetCore::task_id> NetCore::Thread::TaskManager::_taskId = 1; 

NetCore::Thread::TaskManager::TaskManager()
{
	// For main thread.
	_init_tls(_get_new_task_id());
}

NetCore::Thread::TaskManager::~TaskManager()
{
	DESTRUCTOR(TaskManager);

	_join_all_tasks();

	// For main thread
	_task_done();

	_tasks.clear();
}

NetCore::task_id NetCore::Thread::TaskManager::AddTask(std::function<void()> task)
{
	_LOCK_GUARD;

	task_id id = _get_new_task_id();
	auto th = std::thread([=]() {
		_init_tls(id);

		task();

		_task_done();
		});
	_tasks.insert(make_pair(id, make_pair(std::move(th), false)));
	return id;
}

pair<NetCore::task_id, NetCore::task_id> NetCore::Thread::TaskManager::AddTask(std::function<void()> task, const count_t count)
{
	if (count == 1)
	{
		auto id = AddTask(task);
		return { id, id };
	}

	_LOCK_GUARD;

	const task_id first = _taskId.load();

	for (uint32 i = 0; i < count; ++i)
	{
		task_id id = _get_new_task_id();
		auto th = std::thread([=]() {
			_init_tls(id);

			task();

			_task_done();
			});
		_tasks.insert(make_pair(id, make_pair(std::move(th), false)));
	}

	const task_id second = _taskId.load() - 1;

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

void NetCore::Thread::TaskManager::_init_tls(const task_id& id)
{
	TLS_SendBuffer = GSendBufferManager->Pop();
	TLS_Id = id;

	InitTLS();
}

void NetCore::Thread::TaskManager::_task_done()
{
	// without lock
	_tasks.at(TLS_Id).second = true;

	ClearTLS();
}
