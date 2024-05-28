#include "pch.h"
#include "GlobalTimeJobSerializer.h"

void NetCore::GlobalTimeJobSerializer::ExecuteTimeJobs(const uint64 now)
{
#ifndef USE_GLOBAL_JOB_SERIALIZER
	ASSERT_CRASH("USE_GLOBAL_JOB_SERIALIZER is not defined.");
#else
	Vector<TimeJobSPtr> jobs;
		_timeJobs.PopAll(jobs, _check_condiion_time_job(now));

		for (auto& job : jobs)
		{
			job->Execute();
		}

		if (_reserved.load()) GGlobalJobWorker->AddTimeJobQueue(shared_from_this());
		else _reserved.store(false);
#endif // !USE_GLOBAL_JOB_SERIALIZER
}

void NetCore::GlobalTimeJobSerializer::_push(TimeJobSPtr job)
{
#ifndef USE_GLOBAL_JOB_SERIALIZER
	ASSERT_CRASH("USE_GLOBAL_JOB_SERIALIZER is not defined.");
#else
	_timeJobs.Push(job);

		if (_reserved.exchange(true) == false)
		{
			GGlobalJobWorker->AddTimeJobQueue(shared_from_this());
		}
#endif // !USE_GLOBAL_JOB_SERIALIZER
}
