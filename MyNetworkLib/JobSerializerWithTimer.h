#pragma once

NAMESPACE_OPEN(NetCore);

ABSTRACT class JobSerializerWithTimer : public enable_shared_from_this<JobSerializerWithTimer>
{
public:
	virtual ~JobSerializerWithTimer()
	{
#ifdef TEST
		MESSAGE(~JobSerializerWithTimer);
#endif // TEST
	}

	shared_ptr<TimeJob> ReserveJob(std::function<void()> func, const uint64 tickAfter)
	{
		shared_ptr<TimeJob> jobSPtr = NetCore::make_shared<TimeJob>(tickAfter, std::move(func));
		_jobs.push(jobSPtr);
		return static_pointer_cast<TimeJob>(jobSPtr->shared_from_this());
	}

	template<typename T, typename Ret, typename... Args>
	shared_ptr<TimeJob> ReserveJob(const uint64 tickAfter, Ret(T::* pfunc)(Args...), Args&&... args)
	{
		shared_ptr<T> ptr = static_pointer_cast<T>(shared_from_this());
		shared_ptr<TimeJob> jobSPtr = NetCore::make_shared<TimeJob>(tickAfter, ptr, pfunc, std::forward<Args>(args)...);
		
		
		_jobs.push(jobSPtr);
		return static_pointer_cast<TimeJob>(jobSPtr->shared_from_this());
	}

	// TEMP
	void DoWork()
	{
		// assume it is single-thread now
		// And all jobs are reserved before.

		{
			while (_jobs.empty() == false)
			{
				const auto& job = _jobs.top();
				if (::GetTickCount64() > job->GetExecTick())
				{
					job->Execute();
					_jobs.pop();
				}
			}
		}
	}
private:
	PriorityQueue<shared_ptr<TimeJob>> _jobs;
};

NAMESPACE_CLOSE;