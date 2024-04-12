#pragma once

NAMESPACE_OPEN(NetCore::Thread);

using namespace NetCore;

/// <summary>
/// This class is for long-run tasks.
/// <para>Note: Task id will remained until destructor of TaskManager is called.</para>
/// <para>So, adding too many tasks can cause some memory increasements.</para>
/// </summary>
class TaskManager
{
public:
	TaskManager();
	~TaskManager();

	/// <summary>
	/// Create a new thread to do task.
	/// </summary>
	/// <param name="task">Task to do in other thread.</param>
	/// <returns>Task id. (Note: It is not PID.)</returns>
	task_id AddTask(std::function<void()> task);
	/// <summary>
	/// Create threads and each does task.
	/// </summary>
	/// <param name="task">Task to do in other threads.</param>
	/// <param name="count">Count of threads.</param>
	/// <returns>First value is first-created task id and second value is last-created task id.</returns>
	pair<task_id, task_id> AddTask(std::function<void()> task, const count_t count);
	/// <summary>
	/// Checks if the task of the id is running.
	/// <para>Note: It is not lock-safe funtion.</para>
	/// </summary>
	/// <param name="id">Task id to check.</param>
	/// <returns>True if the task is still running, false otherwise.</returns>
	bool IsRunning(const task_id id) const { return _tasks.at(id).second; }
	/// <summary>
	/// Join task of the id.
	/// </summary>
	/// <param name="id">Task id to join.</param>
	/// <returns>True if successful, false if the task is not done or not joinable.</returns>
	bool JoinTask(const task_id id);

public: // virtuals
	
	/// <summary>
	/// Called when main thread and new thread starts.
	/// </summary>
	virtual void InitTLS();

	/// <summary>
	/// Called when main thread and new thread finishes its task.
	/// </summary>
	virtual void ClearTLS();

private:
	inline void _task_done(const task_id id)
	{
		_tasks.at(id).second = true;
	}
	void _join_all_tasks();
private:
	_USE_COMMON_LOCK;
	std::unordered_map<task_id, pair<std::thread, bool>> _tasks;
};

NAMESPACE_CLOSE;