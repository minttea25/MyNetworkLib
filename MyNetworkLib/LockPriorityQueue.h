#pragma once

NAMESPACE_OPEN(NetCore);

template<typename T, typename Less = less<typename Vector<T>::value_type>>
class LockPriorityQueue
{
public:
	LockPriorityQueue() {}
	~LockPriorityQueue() {}

	LockPriorityQueue(const LockPriorityQueue&) = delete;
	LockPriorityQueue(LockPriorityQueue&& other) noexcept = delete;
	LockPriorityQueue& operator=(const LockPriorityQueue&) = delete;
	LockPriorityQueue& operator=(LockPriorityQueue&&) noexcept = delete;

	/// <summary>
	/// Push an item into queue.
	/// </summary>
	/// <param name="item">Item to push into queue.</param>
	void Push(T item)
	{
		_WRITE_LOCK;
		_pq.push(item);
	}

	/// <summary>
	/// Pop and get popped item from queue.
	/// <para>Note: If queue is empty, return T().</para>
	/// </summary>
	/// <returns>Popped item from queue. T() if queue is empty.</returns>
	T Pop()
	{
		_WRITE_LOCK;

		if (_pq.empty())
		{
			if constexpr (std::is_pointer_v<T>) return nullptr;
			else return T();
		}

		T top = _pq.top();
		_pq.pop();
		return top;
	}

	/// <summary>
	/// Try to pop and if successful, get popped item.
	/// </summary>
	/// <param name="item">[OUT] Popped item if successful.</param>
	/// <returns>True if successful, false if queue is empty.</returns>
	bool TryPop(OUT T& item)
	{
		_WRITE_LOCK;

		if (_pq.empty()) return false;
		else
		{
			item = _pq.front();
			_pq.pop();
			return true;
		}
	}

	bool TryPop(OUT T& item, std::function<bool(const T&)> condition)
	{
		_WRITE_LOCK;

		if (_pq.empty()) return false;
		
		const T& top = _pq.top();

		if (condition(top))
		{
			item = top;
			_pq.pop();
			return true;
		}

		return false;
	}

	/// <summary>
	/// Get top item of queue.
	/// <para>Note: It returns T() if queue is empty.</para>
	/// </summary>
	/// <returns>Top item of queue if successful, T() if queue is empty.</returns>
	T Peek() 
	{
		_READ_LOCK;

		if (_pq.empty())
		{
			if constexpr (std::is_pointer_v<T>) return nullptr;
			else return T();
		}
		else return _pq.front();
	}

	/// <summary>
	/// Try to get top item of queue.
	/// </summary>
	/// <param name="front">[OUT] Top item of queue if successful.</param>
	/// <returns>True if successful, false if queue is empty.</returns>
	bool TryPeek(OUT T& top)
	{
		_READ_LOCK;

		if (_pq.empty()) return false;
		else
		{
			top = _pq.top();
			return true;
		}
	}

	/// <summary>
	/// Clear all items in queue.
	/// </summary>
	void Clear()
	{
		_WRITE_LOCK;
		while (_pq.empty() == false)
		{
			_pq.pop();
		}
	}

	/// <summary>
	/// Pop all items from queue and push them to popped.
	/// </summary>
	/// <param name="popped">[OUT] The vector of popped items.</param>
	void Clear(OUT Vector<T>& elements)
	{
		_WRITE_LOCK;
		while (T element = Pop()) elements.push_back(element);
	}

	/// <summary>
	/// Set queue new without popping items.
	/// <para>Note: Be careful to memory leak.</para>
	/// </summary>
	void SetNew()
	{
		_WRITE_LOCK;
		_pq = PriorityQueue<T, Vector<T>, Less>();
	}
private:
	_USE_LOCK;
	PriorityQueue<T, Vector<T>, Less> _pq;
};

NAMESPACE_CLOSE;

