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
	JobSerializer()
	{

	}

	virtual ~JobSerializer()
	{
		DESTRUCTOR(JobSerializer);
	}

	/// <summary>
	/// Make a job with function and push it to queue.
	/// </summary>
	/// <param name="func">function to execute as job</param>
	void PushJob(std::function<void()>&& func)
	{
		_push(NetCore::ObjectPool<Job>::make_shared(std::move(func)));
	}

	/// <summary>
	/// Make a job with given parameters and push it to queue.
	/// </summary>
	/// <typeparam name="T">the type which has the function</typeparam>
	/// <typeparam name="Ret">the return type of the function</typeparam>
	/// <typeparam name="...Args">the types of arugments</typeparam>
	/// <param name="pfunc">the pointer of the function</param>
	/// <param name="...args">the arguments of the function</param>
	template<typename T, typename Ret, typename... Args>
	void PushJob(Ret(T::*pfunc)(Args...), Args&&... args)
	{
		shared_ptr<T> ptr = static_pointer_cast<T>(shared_from_this());
		_push(NetCore::ObjectPool<Job>::make_shared(ptr, pfunc, std::forward<Args>(args)...));
	}

	/// <summary>
	/// Execute all jobs in queue. 
	/// <para>When there is no more job in queue, returns. </para>
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