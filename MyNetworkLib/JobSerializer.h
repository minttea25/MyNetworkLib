#pragma once
#include "Job.h"

NAMESPACE_OPEN(NetCore);

/// <summary>
/// JobSerializer is abstract class for executing jobs in order on one thread.
/// <para>Note: Call Flush() manually.</para>
/// </summary>
ABSTRACT class JobSerializer : public enable_shared_from_this<JobSerializer>
{
public:
	virtual ~JobSerializer()
	{
		DESTRUCTOR(JobSerializer);
	}

	void PushJob(std::function<void()>&& func)
	{
		// TODO : Change make_shared to ObjectPool::make_shared
		//_push(NetCore::make_shared<Job>(std::move(func)));
		_push(NetCore::ObjectPool<Job>::make_shared(std::move(func)));
	}

	template<typename T, typename Ret, typename... Args>
	void PushJob(Ret(T::*pfunc)(Args...), Args&&... args)
	{
		shared_ptr<T> ptr = static_pointer_cast<T>(shared_from_this());
		//_push(NetCore::make_shared<Job>(ptr, pfunc, std::forward<Args>(args)...));
		_push(NetCore::ObjectPool<Job>::make_shared(ptr, pfunc, std::forward<Args>(args)...));
	}

	/// <summary>
	/// Execute all jobs in queue. 
	/// <para>If there is no more job in queue, returns. </para>
	/// </summary>
	void Flush()
	{
		while (true)
		{
			JobSPtr job = nullptr;
			if (_jobs.TryPop(OUT job)) job->Execute();
			else return;
		}
	}

private:
	inline void _push(JobSPtr job)
	{
		_jobs.Push(job);
	}
private:
	LockQueue<JobSPtr> _jobs;
};

NAMESPACE_CLOSE;