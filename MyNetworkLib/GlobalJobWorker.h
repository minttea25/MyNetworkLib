#pragma once
NAMESPACE_OPEN(NetCore);

/// <summary>
/// Is has two queues, one is jobs and the other one is reserved jobs.
/// <para></para>
/// </summary>
class GlobalJobWorker
{
public:
	GlobalJobWorker()
	{
#ifndef USE_GLOBAL_JOB_SERIALIZER
		ASSERT_CRASH("USE_GLOBAL_JOB_SERIALIZER is not defined.");
#endif // !USE_GLOBAL_JOB_SERIALIZER
	}

	~GlobalJobWorker()
	{
		DESTRUCTOR(GlobalJobWorker);
	}

	void AddJobQueue(GlobalJobSerializerSPtr jobQueue)
	{
		_queues.Push(jobQueue);
	}

	GlobalJobSerializerSPtr GetOneJobQueue()
	{
		GlobalJobSerializerSPtr queue = nullptr;
		if (_queues.TryPop(queue) == false) return nullptr;
		else return queue;
	}

	void AddTimeJobQueue(GlobalTimeJobSerializerSPtr jobTimerQueue)
	{
		_timerQueues.Push(jobTimerQueue);
	}

	GlobalTimeJobSerializerSPtr GetOneJobTimerQueue()
	{
		GlobalTimeJobSerializerSPtr queue = nullptr;
		if (_timerQueues.TryPop(queue) == false) return nullptr;
		else return queue;
	}

private:
	LockQueue<GlobalJobSerializerSPtr> _queues;
	LockQueue<GlobalTimeJobSerializerSPtr> _timerQueues;
};

NAMESPACE_CLOSE;

