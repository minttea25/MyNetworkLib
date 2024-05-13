#pragma once
NAMESPACE_OPEN(NetCore);

/// <summary>
/// Is has two queues, one is jobs and the other one is reserved jobs.
/// <para></para>
/// </summary>
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

	/// <summary>
	/// Make the job with func and push it to job queue.
	/// </summary>
	/// <param name="func">function used as a job</param>
	void PushJob(std::function<void()>&& func)
	{
		_push(NetCore::ObjectPool<Job>::make_shared(std::move(func)));
	}

	/// <summary>
	/// Make the job with the func and args and push it to job queue.
	/// </summary>
	/// <typeparam name="T">the type which has the function pointer</typeparam>
	/// <typeparam name="Ret">the return type of the function</typeparam>
	/// <typeparam name="...Args">the types of arguments of the function</typeparam>
	/// <param name="pfunc">the pointer of the function</param>
	/// <param name="...args">the arguments of the function</param>
	template<typename T, typename Ret, typename... Args>
	void PushJob(Ret(T::* pfunc)(Args...), Args&&... args)
	{
		shared_ptr<T> ptr = static_pointer_cast<T>(shared_from_this());
		_push(NetCore::ObjectPool<Job>::make_shared(ptr, pfunc, std::forward<Args>(args)...));
	}

	/// <summary>
	/// Try popping a job from the job queue.
	/// </summary>
	/// <param name="job">[OUT] the job reference</param>
	/// <returns>true if successful, false otherwise</returns>
	bool TryPop(OUT JobSPtr& job)
	{
		return _jobs.TryPop(job);
	}

	/// <summary>
	/// Try popping a reserved job from the queue with the condition.
	/// </summary>
	/// <param name="job">[OUT] the reserved job reference</param>
	/// <param name="now">the current tick</param>
	/// <returns>true if successful, false otherwise</returns>
	bool TryPop(OUT TimeJobSPtr& job, const uint64 now)
	{
		return _timeJobs.TryPop(job, _check_condiion_time_job(now));
	}

	/// <summary>
	/// Make a TimeJob with given parameters and push it to time job queue.
	/// </summary>
	/// <param name="func">the function</param>
	/// <param name="tickAfter">reserved tick count from now</param>
	/// <returns>created TimeJob</returns>
	TimeJobSPtr PushJob(std::function<void()>&& func, const uint64 tickAfter)
	{
		TimeJobSPtr jobSPtr = NetCore::ObjectPool<TimeJob>::make_shared(tickAfter, std::move(func));
		_push(jobSPtr);
		return jobSPtr;
	}

	/// <summary>
	/// Make a TimeJob with given parameters and push it to time job queue.
	/// </summary>
	/// <typeparam name="T">the type which has the function pointer</typeparam>
	/// <typeparam name="Ret">the return type of the function</typeparam>
	/// <typeparam name="...Args">the type of arguments of the function</typeparam>
	/// <param name="tickAfter">reserved tick count from now</param>
	/// <param name="pfunc">pointer of the function</param>
	/// <param name="...args">the arguments of the function</param>
	/// <returns>created TimeJob</returns>
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
	LockPriorityQueue<TimeJobSPtr, TimeJob::TimeJobSPtrComp> _timeJobs;
};

NAMESPACE_CLOSE;

