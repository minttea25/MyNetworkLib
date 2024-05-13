#pragma once

NAMESPACE_OPEN(NetCore);

/// <summary>
/// JobSerializer which executes job at reserved tick time.
/// </summary>
ABSTRACT class JobSerializerWithTimer : public enable_shared_from_this<JobSerializerWithTimer>
{
public:
	virtual ~JobSerializerWithTimer()
	{
		DESTRUCTOR(JobSerializerWithTimer);
	}

	/// <summary>
	/// Make a TimeJob with function and tickAfter.
	/// </summary>
	/// <param name="func">function to reserve</param>
	/// <param name="tickAfter">executable tick count from now</param>
	/// <returns>created TimeJob</returns>
	TimeJobSPtr ReserveJob(std::function<void()>&& func, const uint64 tickAfter)
	{
		TimeJobSPtr jobSPtr = NetCore::ObjectPool<TimeJob>::make_shared(tickAfter, std::move(func));
		_push(jobSPtr);
		return static_pointer_cast<TimeJob>(jobSPtr->shared_from_this());
	}

	/// <summary>
	/// Make a TimeJob with given parameters.
	/// </summary>
	/// <typeparam name="T">the type which has the function</typeparam>
	/// <typeparam name="Ret">the return type of the function</typeparam>
	/// <typeparam name="...Args">the types of the arguments</typeparam>
	/// <param name="tickAfter">executable tick count from now</param>
	/// <param name="pfunc">the pointer of function</param>
	/// <param name="...args">the arguments of funtion</param>
	/// <returns>created TimeJob</returns>
	template<typename T, typename Ret, typename... Args>
	TimeJobSPtr ReserveJob(const uint64 tickAfter, Ret(T::* pfunc)(Args...), Args&&... args)
	{
		shared_ptr<T> ptr = static_pointer_cast<T>(shared_from_this());
		TimeJobSPtr jobSPtr = NetCore::ObjectPool<TimeJob>::make_shared(tickAfter, ptr, pfunc, std::forward<Args>(args)...);
		
		_push(jobSPtr);
		return static_pointer_cast<TimeJob>(jobSPtr->shared_from_this());
	}

	/// <summary>
	/// Execute jobs if the execution time of job is reached.
	/// <para>When there is no more job to execute now, returns.</para>
	/// </summary>
	void Flush()
	{
		while (true)
		{
			uint64 now = ::GetTickCount64();

			TimeJobSPtr job = nullptr;

			if (_jobs.TryPop(OUT job, _check_condiion_time_job(now)) == false) return;
			job->Execute();
		}
	}
private:
	void _push(TimeJobSPtr job)
	{
		_jobs.Push(job);
	}

	static std::function<bool(const TimeJobSPtr&)> _check_condiion_time_job(const uint64 now)
	{
		return [now](const TimeJobSPtr& job) -> bool {
			return job->GetExecTick() < now;
			};
	}
private:
	_USE_LOCK;
	LockPriorityQueue<TimeJobSPtr> _jobs;
};

NAMESPACE_CLOSE;