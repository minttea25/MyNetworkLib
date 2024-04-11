#pragma once

NAMESPACE_OPEN(NetCore::Thread);

using namespace NetCore;

class TaskManager
{
public:
	TaskManager();
	~TaskManager();

	task_id AddTask(std::function<void()> task);
	bool IsRunning(const task_id id) { return !_tasks[id].second; }
public: // virtuals
	virtual void InitTLS();
	virtual void ClearTLS();

private:
	inline void _task_done(const task_id id)
	{
		_tasks[id].second = true;
	}
	void _join_all_tasks();
private:
	_USE_COMMON_LOCK;
	HashMap<task_id, pair<std::thread, bool>> _tasks;
};

NAMESPACE_CLOSE;