#pragma once

NAMESPACE_OPEN(NetCore);

ABSTRACT class GlobalJobSerializer : public enable_shared_from_this<GlobalJobSerializer>
{
public:
	GlobalJobSerializer() 
	{
#ifndef USE_GLOBAL_JOB_SERIALIZER
		ASSERT_CRASH("USE_GLOBAL_JOB_SERIALIZER is not defined.");
#endif // !USE_GLOBAL_JOB_SERIALIZER
	}
	virtual ~GlobalJobSerializer() {}

	void ExecuteJobs()
	{
		Vector<JobSPtr> jobs;
		_jobs.Clear(jobs);

		_queued.store(false);

		if (jobs.size() == 0) return;

		for (auto& job : jobs)
		{
			job->Execute();
		}
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
	void PushJob(Ret(T::* pfunc)(Args...), Args... args)
	{
		shared_ptr<T> ptr = static_pointer_cast<T>(shared_from_this());
		_push(NetCore::ObjectPool<Job>::make_shared(ptr, pfunc, std::forward<Args>(args)...));
	}


private:
	void _push(JobSPtr job);

private:
	LockQueue<JobSPtr> _jobs;

	Atomic<bool> _queued = false;
};

NAMESPACE_CLOSE;




