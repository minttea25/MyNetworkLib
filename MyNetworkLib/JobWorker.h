#pragma once

#include "ReservableJob.h"

NAMESPACE_OPEN(NetCore);

/// <summary>
/// JobWorker provides templates to implement methods of processing serialized-jobs.
/// </summary>
ABSTRACT class JobWorker abstract
{
public:
	JobWorker() {}
	virtual ~JobWorker() {}

	PURE_VIRTUAL virtual void AddJobSerializer(AJobSerializerSPtr jobSerializer) = 0;
	PURE_VIRTUAL virtual void DoJobs() = 0;
	PURE_VIRTUAL virtual void ClearJobs() = 0;

	PURE_VIRTUAL virtual void AddReservableJob(const ReservableJobSPtr job, const AJobSerializerWPtr owner) = 0;
	PURE_VIRTUAL virtual void CheckReservedJob(const uint64 nowTick) = 0;
	PURE_VIRTUAL virtual void ClearReservedJobs() = 0;
private:
	PURE_VIRTUAL virtual AJobSerializerSPtr PopJobSerializer() = 0;
};


/// <summary>
/// GlobalJobWorker can be used with adding jobs or reservedJobs in global of library.
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
	}

	/// <summary>
	/// Add a job in queue.
	/// </summary>
	/// <param name="jobSerializer"></param>
	void AddJobSerializer(AJobSerializerSPtr jobSerializer) override final
	{
		_jobQueue.Push(jobSerializer);
	}

	/// <summary>
	/// Do jobs in queue.
	/// </summary>
	void DoJobs() override final;

	/// <summary>
	/// Clear all jobs in queue.
	/// </summary>
	void ClearJobs() override final
	{
		_jobQueue.Clear();
	}

	/// <summary>
	/// Add reservable job in reservable job queue. (with lock)
	/// </summary>
	/// <param name="job">reservable job to add.</param>
	/// <param name="owner">the owner of the job</param>
	void AddReservableJob(const ReservableJobSPtr job, const AJobSerializerWPtr owner) override final
	{
		WRITE_LOCK(reservableJobQueue);
		//_reservableQueue.push(job);
		_reservableQueue.push(job);
	}

	/// <summary>
	/// Checks the reserved jobs and pushes them into queue.
	/// </summary>
	/// <param name="nowTick">current tick</param>
	void CheckReservedJob(const uint64 nowTick) override final;

	/// <summary>
	/// Clear all reserved jobs (with lock)
	/// </summary>
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
	PriorityQueue< ReservableJobSPtr, Vector<ReservableJobSPtr>, ReservableJobCompare> _reservableQueue;
	Atomic<bool> _executingReservables;

	LockQueue<AJobSerializerSPtr> _jobQueue;
};

NAMESPACE_CLOSE;

