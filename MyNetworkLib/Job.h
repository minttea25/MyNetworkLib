#pragma once

NAMESPACE_OPEN(NetCore);

/// <summary>
/// Job class for JobSerializer.
/// </summary>
class Job : public enable_shared_from_this<Job>
{
public:
	~Job()
	{
#ifdef TEST
		MESSAGE(~Job);
#endif // TEST
	}
	Job(std::function<void()>&& func) : _task(std::move(func)) {}

	template<typename T, typename Ret, typename... Args>
	Job(std::shared_ptr<T> ptr, Ret(T::*pfunc), Args&&... args)
	{
		_task = [ptr, pfunc, args = std::make_tuple(std::forward<Args>(args)...)]() mutable 
			{
			std::apply(
				[ptr, pfunc](Args&&... args) { (ptr.get()->*pfunc)(std::forward<Args>(args)...); }, 
				std::move(args));
			};
	}

	void Execute()
	{
		_task();
	}
private:
	std::function<void()> _task;
};


NAMESPACE_CLOSE;