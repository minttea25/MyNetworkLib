#pragma once

NAMESPACE_OPEN(NetCore);

template<typename T>
class LockQueue
{
public:
	void Push(T item)
	{
		_WRITE_LOCK;
		_queue.push(item);
	}

	T Pop()
	{
		_WRITE_LOCK;

		if (_queue.empty()) return T();

		T front = _queue.front();
		_queue.pop();
		return front;
	}

	T Front()
	{
		_READ_LOCK;

		if (_queue.empty()) return T();

		return _queue.front();
	}

	void Clear()
	{
		_WRITE_LOCK;
		while (_queue.empty() == false)
		{
			_queue.pop();
		}
	}

	void SetNew()
	{
		_WRITE_LOCK;
		_queue = Queue<T>();
	}
private:
	_USE_LOCK;
	Queue<T> _queue;
};

NAMESPACE_CLOSE;

