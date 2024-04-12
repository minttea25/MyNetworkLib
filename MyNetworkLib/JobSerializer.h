#pragma once

NAMESPACE_OPEN(NetCore);

ABSTRACT class JobSerializer : public enable_shared_from_this<JobSerializer>
{
public:
	virtual ~JobSerializer()
	{
#ifdef TEST
		MESSAGE(~JobSerializer);
#endif // TEST
	}

	void PushJob(std::function<void()> func)
	{
		// TODO : Change make_shared to ObjectPool::make_shared
		_jobs.push(NetCore::make_shared<Job>(std::move(func)));
	}

	template<typename T, typename Ret, typename... Args>
	void PushJob(Ret(T::*pfunc)(Args...), Args&&... args)
	{
		shared_ptr<T> ptr = static_pointer_cast<T>(shared_from_this());
		_jobs.push(NetCore::make_shared<Job>(ptr, pfunc, std::forward<Args>(args)...));
	}

	// TEMP
	void DoOneTask()
	{
		if (_jobs.empty()) return;
		auto job = _jobs.front();
		_jobs.pop();

		job.get()->Execute();
	}

	// TEMP
	bool IsEmpty()
	{
		return _jobs.empty();
	}

private:
	Queue<shared_ptr<Job>> _jobs;
};

NAMESPACE_CLOSE;