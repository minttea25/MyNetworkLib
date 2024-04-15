#pragma once

NAMESPACE_OPEN(NetCore);

template<typename T>
class LockStack
{
public:
	void Push(T element)
	{
		_WRITE_LOCK;

		_stack.push(element);
	}

	T Pop()
	{
		_WRITE_LOCK;

		if (_stack.empty()) return T();

		T top = _stack.top();
		_stack.pop();
		return top;
	}

	T Top() const
	{
		_READ_LOCK;

		if (_stack.empty()) return T();

		return _stack.top();
	}

	void Clear()
	{
		_WRITE_LOCK;

		while (_stack.empty() == false)
		{
			_stack.pop();
		}
	}

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

