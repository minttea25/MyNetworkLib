#include "pch.h"
#include "TaskManagerEx.h"

NetCore::Atomic<NetCore::task_id> NetCore::Thread::TaskManagerEx::_taskId = 1;

NetCore::TaskManagerEx::TaskManagerEx()
{
	// For main thread
	_init_tls();
}

NetCore::TaskManagerEx::~TaskManagerEx()
{
	DESTRUCTOR(TaskManagerEx);

	_join_all_tasks();

	ClearTLS();

	_tasks.clear();
}

void NetCore::TaskManagerEx::AddTask(std::function<void()> task)
{
	_LOCK_GUARD;

	auto th = std::thread([=]() {
		_init_tls();

		task();

		_task_done();
		});

	_tasks.push_back(std::move(th));
}

void NetCore::TaskManagerEx::AddTask(std::function<void()> task, const count_t count)
{
	for (uint32 i = 0; i < count; ++i)
	{
		AddTask(task);
	}
}

void NetCore::TaskManagerEx::JoinAllTasks()
{
	_join_all_tasks();
}

#ifdef USE_GLOBAL_JOBQUEUE

void NetCore::TaskManagerEx::DoWorkJob(const uint64 durationTick)
{
	if (_doingJobWorks.exchange(true) == true) return;

	// CRITICAL SECTION-----

	uint64 limit = ::GetTickCount64() + durationTick;

	while (true)
	{
		const uint64 now = ::GetTickCount64();
		if (now > limit) break;

		JobSPtr job = nullptr;
		if (GGlobalJobQueue->TryPop(job) == false) break;

		job->Execute();
	}

	// CRITICAL SECTION-----

	_doingJobWorks.store(false);
}

void NetCore::TaskManagerEx::DoWorkReservedJob(const uint64 durationTick)
{
	if (_doingTimeJobWorks.exchange(true) == true) return;

	// CRITICAL SECTION-----

	uint64 limit = ::GetTickCount64() + durationTick;

	while (true)
	{
		const uint64 now = ::GetTickCount64();
		if (now > limit) break;

		TimeJobSPtr job = nullptr;
		if (GGlobalJobQueue->TryPop(job, now) == false) break;

		job->Execute();
	}

	// CRITICAL SECTION-----

	_doingTimeJobWorks.store(false);
}
#endif // USE_GLOBAL_JOBQUEUE



void NetCore::TaskManagerEx::_init_tls()
{
	TLS_SendBuffer = GSendBufferManager->Pop();
	TLS_Id = _get_new_task_id();

	InitTLS();
}

void NetCore::TaskManagerEx::_task_done()
{
	ClearTLS();
}

void NetCore::TaskManagerEx::_join_all_tasks()
{
	for (auto& t : _tasks)
	{
		if (t.joinable()) t.join();
	}
}
