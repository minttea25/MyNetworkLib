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
		//DESTRUCTOR(TimeJob);
	}

	/// <summary>
	/// Make TimeJob with function.
	/// </summary>
	/// <param name="tickAfter">executable tick count from now</param>
	/// <param name="func">function to reserve as a job</param>
	TimeJob(const uint64 tickAfter, std::function<void()>&& func) : _task(std::move(func))
	{
		_execTick = ::GetTickCount64() + tickAfter;
	}

	/// <summary>
	/// Make TimeJob with given parameters
	/// </summary>
	/// <typeparam name="T">the type which has function</typeparam>
	/// <typeparam name="Ret">the return type of function</typeparam>
	/// <typeparam name="...Args">the types of the arguments</typeparam>
	/// <param name="tickAfter">executable tick count from now.</param>
	/// <param name="ptr">the pointer of owner</param>
	/// <param name="pfunc">the pointer of function</param>
	/// <param name="...args">the arguments of function</param>
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

	/// <summary>
	/// Get reserved tick count of the job.
	/// </summary>
	/// <returns>reserved tick count</returns>
	uint64 GetExecTick() const { return _execTick; }

	inline bool operator<(const TimeJob& other) const
	{
		return _execTick > other._execTick;
	}

	/// <summary>
	/// Execute the job
	/// </summary>
	void Execute()
	{
		_task();
	}

	struct TimeJobComp
	{
		bool operator()(const TimeJob& lhs, const TimeJob& rhs)
		{
			return lhs._execTick > rhs._execTick;
		}
	};

	struct TimeJobSPtrComp
	{
		bool operator()(const std::shared_ptr<TimeJob>& lhs, const std::shared_ptr<TimeJob>& rhs)
		{
			return lhs->_execTick > rhs->_execTick;
		}
	};
private:
	uint64 _execTick;
	std::function<void()> _task;
};

NAMESPACE_CLOSE;
