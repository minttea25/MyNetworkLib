#include "pch.h"

void NetCore::ReservableJob::PushJobToSerializer()
{
	ASSERT_CRASH(_jobData != nullptr);

	if (_canceled == false)
	{
		if (auto owner = _jobData->_owner.lock())
		{
			owner->Push(_jobData->_job);
		}
	}

	ObjectPool<JobData>::Release(_jobData);
}
