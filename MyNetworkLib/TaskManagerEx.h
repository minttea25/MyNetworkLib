#pragma once
NAMESPACE_OPEN(NetCore);

class TaskManagerEx
{
public:
	TaskManagerEx();
	~TaskManagerEx();

	void AddTask(std::function<void()> task);

	void AddTask(std::function<void()> task, const count_t count);

	void JoinAllTasks();

	void DoWorkFromGlobalJobQueue(const uint64 durationTick);
	
	void DoWorkJob(const uint64 durationTick);

	void DoWorkReservedJob();

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
};

NAMESPACE_CLOSE;

