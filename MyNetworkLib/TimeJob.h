#pragma once

NAMESPACE_OPEN(NetCore);

/// <summary>
/// Job class which has execution tick time for JobSerializerWithTimer.
/// </summary>
class TimeJob : public enable_shared_from_this<TimeJob>
{
public:
	~TimeJob()
	{
		DESTRUCTOR(TimeJob);
	}

	TimeJob(const uint64 tickAfter, std::function<void()>&& func) : _task(std::move(func))
	{
		_execTick = ::GetTickCount64() + tickAfter;
	}

	template<typename T, typename Ret, typename... Args>
	TimeJob(uint64 tickAfter, std::shared_ptr<T> ptr, Ret(T::*pfunc), Args&&... args)
	{
		_execTick = ::GetTickCount64() + tickAfter;
		_task = [ptr, pfunc, args = std::make_tuple(std::forward<Args>(args)...)]() mutable {
			std::apply(
				[ptr, pfunc](Args&&... args) { (ptr.get()->*pfunc)(std::forward<Args>(args)...); },
				std::move(args));
			};
	}

	uint64 GetExecTick() const { return _execTick; }

	inline bool operator<(const TimeJob& other) const
	{
		return _execTick > other._execTick;
	}

	void Execute()
	{
		_task();
	}
private:
	uint64 _execTick;
	std::function<void()> _task;
};

NAMESPACE_CLOSE;
