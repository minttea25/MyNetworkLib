#pragma once

NAMESPACE_OPEN(NetCore);

/// <summary>
/// Job class for JobSerializer.
/// </summary>
class Job : public enable_shared_from_this<Job>
{
public:
	virtual ~Job() {}

	/// <summary>
	/// Make a job with given function.
	/// </summary>
	/// <param name="func"></param>
	Job(std::function<void()>&& func) : _task(std::move(func)) {}

	/// <summary>
	/// Make a job with given parameters.
	/// </summary>
	/// <typeparam name="T">the type which has the function</typeparam>
	/// <typeparam name="Ret">the return type of the function</typeparam>
	/// <typeparam name="...Args">the types of the arguments</typeparam>
	/// <param name="ptr">the shared pointer of the owner</param>
	/// <param name="pfunc">the pointer of the function</param>
	/// <param name="...args">arguments of the function</param>
	template<typename T, typename Ret, typename... Args>
	Job(std::shared_ptr<T> ptr, Ret(T::* pfunc)(Args...), Args&&... args);
	
	/// <summary>
	/// Execute the job.
	/// </summary>
	void Execute() { if (_task != nullptr) _task(); }
private:
	std::function<void()> _task;
};



template<typename T, typename Ret, typename ...Args>
inline Job::Job(std::shared_ptr<T> ptr, Ret(T::* pfunc)(Args...), Args && ...args)
{
	_task = [ptr, pfunc, args = std::make_tuple(std::forward<Args>(args)...)]() mutable {
		std::apply(
			[ptr, pfunc](Args&&... _args) { (ptr.get()->*pfunc)(std::forward<Args>(_args)...); },
			std::move(args));
		};
}
NAMESPACE_CLOSE;