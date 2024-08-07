#pragma once
NAMESPACE_OPEN(NetCore);

/// <summary>
/// TaskManager using GlobalJobWorker
/// </summary>
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
	void AddTask(std::function<void()> task, const uint32 count);

	/// <summary>
	/// Called when main thread and new thread finishes its task.
	/// </summary>
	void JoinAllTasks();

#ifdef USE_GLOBAL_JOB_SERIALIZER
	/// <summary>
	/// Execute jobs in the GlobalJobQueue for duration.
	/// </summary>
	void DoWorkJob();

	/// <summary>
	/// Push reserved executable jobs to owner(Job Serializer).
	/// </summary>
	void CheckReservedJob();
#endif // USE_GLOBAL_JOBQUEUE

public: // virtuals

	/// <summary>
	/// Called when this class is created and new thread starts.
	/// </summary>
	virtual void InitTLS() {};

	/// <summary>
	/// Called when this class is expired and new thread finishes its task.
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
};

NAMESPACE_CLOSE;

