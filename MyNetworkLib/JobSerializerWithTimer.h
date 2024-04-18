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

	TimeJobSPtr ReserveJob(std::function<void()>&& func, const uint64 tickAfter)
	{
		//TimeJobSPtr jobSPtr = NetCore::make_shared<TimeJob>(tickAfter, std::move(func));
		TimeJobSPtr jobSPtr = NetCore::ObjectPool<TimeJob>::make_shared(tickAfter, std::move(func));
		_push(jobSPtr);
		return static_pointer_cast<TimeJob>(jobSPtr->shared_from_this());
	}

	template<typename T, typename Ret, typename... Args>
	TimeJobSPtr ReserveJob(const uint64 tickAfter, Ret(T::* pfunc)(Args...), Args&&... args)
	{
		shared_ptr<T> ptr = static_pointer_cast<T>(shared_from_this());
		//TimeJobSPtr jobSPtr = NetCore::make_shared<TimeJob>(tickAfter, ptr, pfunc, std::forward<Args>(args)...);
		TimeJobSPtr jobSPtr = NetCore::ObjectPool<TimeJob>::make_shared(tickAfter, ptr, pfunc, std::forward<Args>(args)...);
		
		_push(jobSPtr);
		return static_pointer_cast<TimeJob>(jobSPtr->shared_from_this());
	}

	/// <summary>
	/// Execute jobs if the execution time of job is reached.
	/// <para>If there is no more job to execute now, returns.</para>
	/// </summary>
	void Flush()
	{
		while (true)
		{
			uint64 now = ::GetTickCount64();

			TimeJobSPtr job = nullptr;
			{
				if (_jobs.TryPeek(OUT job) == false) return;
				if (job->GetExecTick() >= now) return;
				DISCARD _jobs.Pop();
			}
			job->Execute();
		}
	}
private:
	void _push(TimeJobSPtr job)
	{
		_jobs.Push(job);
	}
private:
	_USE_LOCK;
	LockPriorityQueue<TimeJobSPtr> _jobs;
};

NAMESPACE_CLOSE;