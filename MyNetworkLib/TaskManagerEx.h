#pragma once
NAMESPACE_OPEN(NetCore);

class TaskManagerEx
{
public:
	TaskManagerEx();
	~TaskManagerEx();

	/// <summary>
	/// Create a new thread to do task.
	/// </summary>
	/// <param name="task">Task to do in new thread.</param>
	void AddTask(std::function<void()> task);
	/// <summary>
	/// Create threads and each does task.
	/// </summary>
	/// <param name="task">Task to do in other threads.</param>
	/// <param name="count">Count of threads.</param>
	void AddTask(std::function<void()> task, const count_t count);

	void JoinAllTasks();

#ifdef USE_GLOBAL_JOB_SERIALIZER
	/// <summary>
	/// Execute jobs in the GlobalJobQueue for duration.
	/// </summary>
	void DoWorkJob();

	/// <summary>
	/// Execute reserved jobs in GlobalJobQueue for duration.
	/// </summary>
	/// <param name="durationTick">execution tick duration</param>
	void DoWorkReservedJob(const uint64 durationTick);
#endif // USE_GLOBAL_JOBQUEUE

public: // virtuals

	/// <summary>
	/// Called when main thread and new thread starts.
	/// </summary>
	virtual void InitTLS() {};

	/// <summary>
	/// Called when main thread and new thread finishes its task.
	/// </summary>
	virtual void ClearTLS() {};
private:
	void _init_tls();
	void _task_done();
	void _join_all_tasks();
	inline task_id _get_new_task_id()
	{
		return _taskId.fetch_add(1);
	}
private:
	_USE_COMMON_LOCK;
	Vector<std::thread> _tasks;
	static Atomic<task_id> _taskId;

#ifdef USE_GLOBAL_JOB_SERIALIZER
	Atomic<bool> _doingJobWorks = false;
	Atomic<bool> _doingTimeJobWorks = false;
#endif // USE_GLOBAL_JOBQUEUE
};

NAMESPACE_CLOSE;

