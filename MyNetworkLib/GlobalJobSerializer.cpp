#include "pch.h"
#include "GlobalJobSerializer.h"

void NetCore::GlobalJobSerializer::_push(JobSPtr job)
{
#ifndef USE_GLOBAL_JOB_SERIALIZER
	ASSERT_CRASH("USE_GLOBAL_JOB_SERIALIZER is not defined.");
#else
	_jobs.Push(job);

		if (_queued.exchange(true) == false)
		{
			GGlobalJobWorker->AddJobQueue(shared_from_this());
		}
#endif // !USE_GLOBAL_JOB_SERIALIZER
}
