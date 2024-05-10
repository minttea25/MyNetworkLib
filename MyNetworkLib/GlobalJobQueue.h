#pragma once
NAMESPACE_OPEN(NetCore);

class GlobalJobQueue : enable_shared_from_this<GlobalJobQueue>
{
public:
	GlobalJobQueue()
	{

	}

	~GlobalJobQueue()
	{
		DESTRUCTOR(GlobalJobQueue);
	}

	void PushJob(std::function<void()>&& func)
	{
		_push(NetCore::ObjectPool<Job>::make_shared(std::move(func)));
	}

	template<typename T, typename Ret, typename... Args>
	void PushJob(Ret(T::* pfunc)(Args...), Args&&... args)
	{
		shared_ptr<T> ptr = static_pointer_cast<T>(shared_from_this());
		_push(NetCore::ObjectPool<Job>::make_shared(ptr, pfunc, std::forward<Args>(args)...));
	}

	JobSPtr PopJob()
	{
		return _jobs.Pop();
	}

	bool TryPop(OUT JobSPtr& job)
	{
		return _jobs.TryPop(job);
	}

	bool TryPop(OUT TimeJobSPtr& job, const uint64 now)
	{
		return _timeJobs.TryPop(job, _check_condiion_time_job(now));
	}

	TimeJobSPtr GetExecutableTimeJob(const uint64 time)
	{
		TimeJobSPtr job = nullptr;
		if (_timeJobs.TryPeek(job) == false) return nullptr;
		if (job->GetExecTick() < time) return nullptr;
		_timeJobs.Pop();
		return job;
	}

	TimeJobSPtr PushJob(std::function<void()>&& func, const uint64 tickAfter)
	{
		TimeJobSPtr jobSPtr = NetCore::ObjectPool<TimeJob>::make_shared(tickAfter, std::move(func));
		_push(jobSPtr);
		return jobSPtr;
	}

	template<typename T, typename Ret, typename... Args>
	TimeJobSPtr PushJob(const uint64 tickAfter, Ret(T::*pfunc)(Args...), Args&&... args)
	{
		shared_ptr<T> ptr = static_pointer_cast<T>(shared_from_this());
		TimeJobSPtr jobSPtr = NetCore::ObjectPool<TimeJob>::make_shared(tickAfter, ptr, pfunc, std::forward<Args>(args)...);
		
		_push(jobSPtr);
		return jobSPtr;
	}

private:
	void _push(JobSPtr job)
	{
		_jobs.Push(job);
	}

	void _push(TimeJobSPtr timeJob)
	{
		_timeJobs.Push(timeJob);
	}

	static std::function<bool(const TimeJobSPtr&)> _check_condiion_time_job(const uint64 now)
	{
		return [now](const TimeJobSPtr& job) -> bool 
			{ 
				return job->GetExecTick() < now; 
			};
	}

private:
	USE_LOCK(timeJob);
	LockQueue<JobSPtr> _jobs;
	LockPriorityQueue<TimeJobSPtr> _timeJobs;
	Atomic<bool> _doingJobWorks = false;
	Atomic<bool> _doingTimeJobWorks = false;
};

NAMESPACE_CLOSE;

