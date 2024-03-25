#pragma once

NAMESPACE_OPEN(NetCore);

class IOCPCore
{
public:
	IOCPCore();
	~IOCPCore();

	HANDLE GetHandle() const { return _iocpHandle; }

	bool RegisterIOCP(class IOCPObject* iocpObject);
	bool GetQueuedCompletionStatus(DWORD dwMilliseconds = INFINITE);
private:
	HANDLE _iocpHandle = NULL; // Note: HANDLE is void*
	ULONG_PTR _completeionKey = 0;
};

NAMESPACE_CLOSE;