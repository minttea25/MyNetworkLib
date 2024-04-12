#pragma once

#include "CoreTypes.h"

NAMESPACE_OPEN(NetCore);

class WriteLock
{
public:
	WriteLock(Lock& lock) : _lock(lock) { _lock.WriteLock(); }
	~WriteLock() { _lock.WriteUnlock(); }
private:
	Lock& _lock;
};

class ReadLock
{
public:
	ReadLock(Lock& lock) : _lock(lock) { _lock.ReadLock(); }
	~ReadLock() { _lock.ReadUnlock(); }
private:
	Lock& _lock;
};

NAMESPACE_CLOSE;

