#include "pch.h"

void NetCore::JobSerializer::ExecuteJobs()
{
	TLS_CurrentJobSerializer = shared_from_this();

	const uint64 startTick = ::GetTickCount64();
	while (true)
	{
		Vector<JobSPtr> jobs;
		// Clear is thread_safe (lock)
		_jobs.Clear(OUT jobs);

		// At this moment, external jobs can be added to this queue.
		const size_t currentCount = jobs.size();
		for (auto& job : jobs)
		{
			job->Execute();
		}

#pragma warning(disable: 4267)
		// If there is no more added job during executing the previous jobs,
		if (_queuedCount.fetch_sub(currentCount) == currentCount)
		{
			// end executing

			// It must set reference nullptr!
			TLS_CurrentJobSerializer = nullptr;
			break;
		}
#pragma warning(default: 4267)
		const uint64 nowTick = ::GetTickCount64();
		// If execution time is passed than more executioTickCount
		if (nowTick - startTick >= TLS_GlobalJobsExecutionMaxTickCount)
		{
			TLS_CurrentJobSerializer = nullptr;
			// Add JobSerializer(this) again to execute next time on this thread or others.
			GGlobalJobWorker->AddJobSerializer(shared_from_this());
			break;
		}

		// Try to execute new-added jobs again.

}

	TLS_CurrentJobSerializer = nullptr;
}

void NetCore::JobSerializer::Push(JobSPtr job)
{
#ifndef USE_GLOBAL_JOB_SERIALIZER
	ASSERT_CRASH("USE_GLOBAL_JOB_SERIALIZER is not defined.");
#else

	// Set added job count + 1
	const auto prevCount = _queuedCount.fetch_add(1);
	_jobs.Push(job);

	if (prevCount == 0)
	{
		// If no jobs are in queue before and this thread is not executing JobSerializers now,
		// starts executing.
		if (TLS_CurrentJobSerializer == nullptr)
		{
			ExecuteJobs();
		}
		// Otherwise, let other thread to do executing.
		else
		{
			GGlobalJobWorker->AddJobSerializer(shared_from_this());
		}
	}
#endif // !USE_GLOBAL_JOB_SERIALIZER
}
