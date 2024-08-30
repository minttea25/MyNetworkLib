#include "pch.h"


void NetCore::GlobalJobWorker::DoJobs()
{
	auto start = ::GetTickCount64();
	while (true)
	{
		if (::GetTickCount64() - start > TLS_GlobalJobsExecutionMaxTickCount) return;

		AJobSerializerSPtr s = PopJobSerializer();
		if (s == nullptr) return;

		s->ExecuteJobs();
	}
}

void NetCore::GlobalJobWorker::CheckReservedJob(const uint64 nowTick)
{
	if (_executingReservables.exchange(true) == true) return;

	Vector<ReservableJobSPtr> jobs;
	{
		// copy with lock
		WRITE_LOCK(reservableJobQueue);

		while (_reservableQueue.empty() == false)
		{
			const auto& job = _reservableQueue.top();
			if (job->GetExecutionTick() > nowTick) break;

			//if (job.IsCanceled()) continue;

			// If the job is executable now, add it to jobs.
			jobs.push_back(job);
			_reservableQueue.pop();
		}

		// Release write lock
	}

	for (auto& job : jobs)
	{
		// If the owner(JobSerializer) is valid), add the job to actual queue.
		job->PushJobToSerializer();
	}

	_executingReservables.store(false);
}