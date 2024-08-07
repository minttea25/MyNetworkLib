#pragma once
NAMESPACE_OPEN(NetCore);

/// <summary>
/// Is has two queues, one is jobs and the other one is reserved jobs.
/// <para></para>
/// </summary>
class GlobalJobWorker final : public JobWorker
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

	void AddJobSerializer(AJobSerializerSPtr jobSerializer) override final
	{
		_jobQueue.Push(jobSerializer);
	}

	void DoJobs() override final;

	void ClearJobs() override final
	{
		_jobQueue.Clear();
	}
	
	ReservableJob& AddReservableJob(const uint64 tickAfter, const JobSPtr job, const AJobSerializerWPtr owner) override final
	{
		ReservableJob rJob(tickAfter + ::GetTickCount64(), job, owner);

		WRITE_LOCK(reservableJobQueue);
		_reservableQueue.push(rJob);

		return rJob;
	}

	void CheckReservedJob(const uint64 nowTick) override final;

	void ClearReservedJobs() override final
	{
		WRITE_LOCK(reservableJobQueue);

		// no clear method in pq
		while (_reservableQueue.empty() == false)
		{
			_reservableQueue.pop();
		}
	}


private:
	AJobSerializerSPtr PopJobSerializer() override final
	{
		AJobSerializerSPtr queue = nullptr;
		DISCARD _jobQueue.TryPop(queue);
		return queue;
	}

private:
	USE_LOCK(reservableJobQueue);
	PriorityQueue<ReservableJob> _reservableQueue;
	Atomic<bool> _executingReservables;

	LockQueue<AJobSerializerSPtr> _jobQueue;
};

NAMESPACE_CLOSE;

