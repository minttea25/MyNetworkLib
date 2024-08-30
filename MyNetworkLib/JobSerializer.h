#pragma once

#include "Job.h"
#include "ReservableJob.h"
#include "JobSerializer.h"
#include "JobWorker.h"

NAMESPACE_OPEN(NetCore);

/// <summary>
/// AJobSerializer is an abstract class providing to serialize jobs.
/// </summary>
ABSTRACT class AJobSerializer : public enable_shared_from_this<AJobSerializer>
{
public:
	AJobSerializer()
	{

	}

	virtual ~AJobSerializer()
	{
		//DESTRUCTOR(AJobSerializer);
	}

	/// <summary>
	/// Make a job with function and push it to queue.
	/// </summary>
	/// <param name="func">function to execute as job</param>
	void PushJob(std::function<void()>&& func)
	{
		Push(NetCore::ObjectPool<Job>::make_shared(std::move(func)));
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
		Push(NetCore::ObjectPool<Job>::make_shared(ptr, pfunc, std::forward<Args>(args)...));
	}

	//template<typename T, typename Ret, typename... Args>
	//PURE_VIRTUAL virtual ReservableJob& PushReservableJob(const uint64 tickAfter, Ret(T::* pfunc)(Args...), Args... args) = 0;

	/// <summary>
	/// Push job to queue.
	/// </summary>
	/// <param name="job"></param>
	PURE_VIRTUAL virtual void Push(JobSPtr job) = 0;

	/// <summary>
	/// Execute jobs in specific ways.
	/// </summary>
	PURE_VIRTUAL virtual void ExecuteJobs() = 0;

};

/// <summary>
/// JobSerializer is a base class using GlobalJobWorker in library as reserve jobs.
/// </summary>
class JobSerializer : public AJobSerializer
{
public:
	JobSerializer() 
	{
#ifndef USE_GLOBAL_JOB_SERIALIZER
		ASSERT_CRASH("USE_GLOBAL_JOB_SERIALIZER is not defined.");
#endif // !USE_GLOBAL_JOB_SERIALIZER
	}

	~JobSerializer()
	{
		_jobs.Clear();
	}

	/// <summary>
	/// Executes pended jobs in queue during `TLS_GlobalJobsExecutionMaxTickCount`(This is mutable) time. 
	/// </summary>
	void ExecuteJobs() override;
	/// <summary>
	/// Push jobs to queue.
	/// </summary>
	/// <param name="job">job to execute</param>
	void Push(JobSPtr job) override;

	/// <summary>
	/// Creates a reservable job and pushes it to GGlobalJobWorker of library.
	/// </summary>
	template<typename T, typename Ret, typename... Args>
	ReservableJobSPtr PushReservableJob(const uint64 tickAfter, Ret(T::* pfunc)(Args...), Args... args)
	{
		std::shared_ptr<T> obj = static_pointer_cast<T>(shared_from_this());
		JobSPtr job = ObjectPool<Job>::make_shared(obj, pfunc, std::forward<Args>(args)...);
		ReservableJobSPtr rJob = ObjectPool<ReservableJob>::make_shared(tickAfter + ::GetTickCount64(), job, shared_from_this());
		GGlobalJobWorker->AddReservableJob(rJob, shared_from_this());

		return rJob;
	}
public:
	/// <summary>
	/// The default value that decides duration a thread check reserved jobs.
	/// </summary>
	static constexpr uint64 DEFAULT_EXECUTION_TICKCOUNT = 64;

private:
	LockQueue<JobSPtr> _jobs;
	Atomic<int> _queuedCount = 0;
};

NAMESPACE_CLOSE;




