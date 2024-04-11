#pragma once

NAMESPACE_OPEN(NetCore::Thread);

using namespace NetCore;

class ThreadPool
{
public:
	const uint32 MAX_POOL_COUNT;

	ThreadPool(const uint32 maxPoolCount);
	~ThreadPool();

    void Stop() { _stop = true; }
	
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<typename std::invoke_result<F, Args...>::type>
    {
        using return_type = typename std::invoke_result<F, Args...>::type;

        auto task = std::make_shared<std::packaged_task<return_type(Args...)>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        auto res = task->get_future();
        {
            std::unique_lock<Mutex> lock(_mutex);
            if (_stop)
            {
                throw std::runtime_error("Pool is already stopped.");
            }
            _waiting_queue.emplace([=]() { (*task)(args...); });
        }
        _condition.notify_one();
        return res;
    }
private:
	std::vector<std::thread> _workers;
	std::queue<std::function<void()>> _waiting_queue;
	Mutex _mutex;
	CondVar _condition;
    bool _stop = false;
};

NAMESPACE_CLOSE;