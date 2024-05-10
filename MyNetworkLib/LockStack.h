#pragma once

NAMESPACE_OPEN(NetCore);

template<typename T>
class LockStack
{
public:
	/// <summary>
	/// Push an item into stack.
	/// </summary>
	/// <param name="item">ITem to push into stack.</param>
	void Push(T item)
	{
		_WRITE_LOCK;

		_stack.push(item);
	}

	/// <summary>
	/// Pop and get popped item from stack.
	/// <para>Note: If stack is empty, return T().</para>
	/// </summary>
	/// <returns>Popped item from stack. T() if stack is empty.</returns>
	T Pop()
	{
		_WRITE_LOCK;

		if (_stack.empty())
		{
			if constexpr (std::is_class_v(T)) return nullptr;
			else return T();
		}

		T top = _stack.top();
		_stack.pop();
		return top;
	}

	/// <summary>
	/// Try to pop and if sucessful, get popped item.
	/// </summary>
	/// <param name="item">[OUT] Popped item if successful.</param>
	/// <returns>True if successfu, false if stack is empty.</returns>
	bool TryPop(OUT T& item)
	{
		_WRITE_LOCK;

		if (_stack.empty()) return false;
		else
		{
			item = _stack.top();
			_stack.pop();
			return true;
		}
	}

	/// <summary>
	/// Get top item of stack.
	/// <para>Note: It returns T() if stack is empty.</para>
	/// </summary>
	/// <returns>Top item of stack if successful, T() if stack is empty.</returns>
	T Top()
	{
		_READ_LOCK;

		if (_stack.empty())
		{
			if constexpr (std::is_class_v(T)) return nullptr;
			else return T();
		}

		return _stack.top();
	}

	/// <summary>
	/// Try to get top item of stack.
	/// </summary>
	/// <param name="top">[OUT] Top item of stack if successful.</param>
	/// <returns>True if successful, false if queue is empty.</returns>
	bool TryPeek(OUT T& top)
	{
		_READ_LOCK;
		if (_stack.empty()) return false;
		else
		{
			top = _stack.top();
			return true;
		}
	}

	/// <summary>
	/// Clear all items in stack.
	/// </summary>
	void Clear()
	{
		_WRITE_LOCK;

		while (_stack.empty() == false)
		{
			_stack.pop();
		}
	}
	
	/// <summary>
	/// Pop all items from stack and push them to popped.
	/// </summary>
	/// <param name="popped">[OUT] The vector of popped items.</param>
	void Clear(OUT Vector<T>& popped)
	{
		_WRITE_LOCK;
		while (T& element = Pop()) popped.push_back(element);
	}

	/// <summary>
	/// Set stack new without popping items.
	/// <para>Note: Be careful to memory leak.</para>
	/// </summary>
	void SetNew()
	{
		_WRITE_LOCK;
		_stack = Stack<T>();
	}
private:
	_USE_LOCK;
	Stack<T> _stack;
};

NAMESPACE_CLOSE;

