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

void NetCore::TaskManagerEx::AddTask(std::function<void()> task, const uint32 count)
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

#ifdef USE_GLOBAL_JOB_SERIALIZER

void NetCore::TaskManagerEx::DoWorkJob()
{
	GGlobalJobWorker->DoJobs();
}

void NetCore::TaskManagerEx::CheckReservedJob()
{
	GGlobalJobWorker->CheckReservedJob(::GetTickCount64());
}
#endif // USE_GLOBAL_JOBQUEUE



void NetCore::TaskManagerEx::_init_tls()
{
	TLS_SendBuffer = GSendBufferManager->Pop();
	TLS_Id = _get_new_task_id();
	TLS_CurrentJobSerializer = nullptr;
	TLS_GlobalJobsExecutionMaxTickCount = JobSerializer::DEFAULT_EXECUTION_TICKCOUNT;

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
