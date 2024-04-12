#include "pch.h"
#include "RWLock.h"

void NetCore::RWLock::WriteLock()
{
	// When the write flag is on same thread, give lock to that thread.
	const uint32 lockThreadId = _get_lock_thread_id();
	if (TLS_Id == lockThreadId)
	{
		_writeCount++;
		return;
	}

	// When the flag is empty, race condition.
	const uint64 beginTick = ::GetTickCount64();

	const uint32 desired = _get_write_desired_flag();
	// Spin to max tick
	while (true)
	{
		// Count spin count
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; ++spinCount)
		{
			// Wait for empty flag
			uint32 expected = EMPTY;
			if (_lockFlag.compare_exchange_strong(OUT expected, desired))
			{
				// Finally get lock.
				_writeCount++;
				return;
			}
			// Or continue
		}

		const uint64 endTick = ::GetTickCount64();
		if (endTick - beginTick >= TIMEOUT_TICK)
		{
			// TIMEOUT to get lock
			ErrorHandler::AssertCrash(Errors::THREAD_LOCK_TIMEOUT);
		}

		this_thread::yield();
	}
}

void NetCore::RWLock::WriteUnlock()
{
	// Check the flag is on read-flag. If the flag is on read-flag, it is logical error!
	// It means a thread is trying to release lock when other thread is on reading.
	// Therefore, it is wrong order because a thread can not be on writing when other thread is on reading.
	if (_get_read_lock_count() != 0)
	{
		ErrorHandler::AssertCrash(Errors::THREAD_LOCK_INVALID_WRITE_UNLOCK_ORDER);
	}

	// It is same thread as the write-flag has now.
	const int32 lockCount = --_writeCount;
	// Check if this is last call for unlocking write lock.
	if (lockCount == 0)
	{
		// Note : Read count is 0 now.
		_lockFlag.store(EMPTY);
	}
}

void NetCore::RWLock::ReadLock()
{
	// Give lock when the same thread has lock.
	const uint32 lockThreadId = _get_lock_thread_id();
	if (TLS_Id == lockThreadId)
	{
		// Note: The lower bit of the flag is the read area.
		_lockFlag.fetch_add(1);
		return;
	}

	// If the flag is empty, race condition
	const uint64 beginTick = ::GetTickCount64();
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; ++spinCount)
		{
			uint32 expected = _get_read_lock_count();
			if (_lockFlag.compare_exchange_strong(OUT expected, expected + 1))
			{
				// It meams this thread finally get the lock to read.
				// Note: compare_exchange_strong contains adding reading count
				return;
			}
		}

		const uint64 endTick = ::GetTickCount64();
		if (endTick - beginTick >= TIMEOUT_TICK)
		{
			ErrorHandler::AssertCrash(Errors::THREAD_LOCK_TIMEOUT);
		}

		this_thread::yield();
	}
}

void NetCore::RWLock::ReadUnlock()
{
	// Substract 1 at reading count
	const uint32 prevReadCount = (_lockFlag.fetch_sub(1) & READ_COUNT_MASK);
	// If prevReadCount is 0, it means a thread is trying to unlock read lock in several.
	if (prevReadCount == 0)
	{
		ErrorHandler::AssertCrash(Errors::THREAD_LOCK_MULTIPLE_READ_UNLOCK);
	}
}
