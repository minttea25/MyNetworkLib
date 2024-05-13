#pragma once

NAMESPACE_OPEN(NetCore);

template<typename T>
class LockQueue
{
public:
	/// <summary>
	/// Push an item into queue.
	/// </summary>
	/// <param name="item">Item to push into queue.</param>
	void Push(T item)
	{
		_WRITE_LOCK;
		_queue.push(item);
	}

	/// <summary>
	/// Pop and get popped item from queue.
	/// <para>Note: If queue is empty, return T().</para>
	/// </summary>
	/// <returns>Popped item from queue. T() if queue is empty.</returns>
	T Pop()
	{
		_WRITE_LOCK;

		if (_queue.empty())
		{
			if constexpr (std::is_pointer_v<T>) return nullptr;
			else return T();
		}

		T front = _queue.front();
		_queue.pop();
		return front;
	}

	/// <summary>
	/// Try to pop and if successful, get popped item.
	/// </summary>
	/// <param name="item">[OUT] Popped item if successful.</param>
	/// <returns>True if successful, false if queue is empty.</returns>
	bool TryPop(OUT T& item)
	{
		_WRITE_LOCK;

		if (_queue.empty()) return false;
		else
		{
			item = _queue.front();
			_queue.pop();
			return true;
		}
	}

	/// <summary>
	/// Get front item of queue.
	/// <para>Note: It returns T() if queue is empty.</para>
	/// </summary>
	/// <returns>Front item of queue if successful, T() if queue is empty.</returns>
	T Front()
	{
		_READ_LOCK;

		if (_queue.empty())
		{
			if constexpr (std::is_pointer_v<T>) return nullptr;
			else return T();
		}

		return _queue.front();
	}

	/// <summary>
	/// Try to get front item of queue.
	/// </summary>
	/// <param name="front">[OUT] Front item of queue if successful.</param>
	/// <returns>True if successful, false if queue is empty.</returns>
	bool TryPeek(OUT T& front)
	{
		_READ_LOCK;

		if (_queue.empty()) return false;
		else
		{
			front = _queue.front();;
			return true;
		}
	}

	/// <summary>
	/// Clear all items in queue.
	/// </summary>
	void Clear()
	{
		_WRITE_LOCK;
		while (_queue.empty() == false)
		{
			_queue.pop();
		}
	}

	/// <summary>
	/// Pop all items from queue and push them to popped.
	/// </summary>
	/// <param name="popped">[OUT] The vector of popped items.</param>
	void Clear(OUT Vector<T>& popped)
	{
		_WRITE_LOCK;
		while (T& element = Pop()) popped.push_back(element);
	}

	/// <summary>
	/// Set queue new without popping items.
	/// <para>Note: Be careful to memory leak.</para>
	/// </summary>
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

