#pragma once

NAMESPACE_OPEN(NetCore);

class IOCPObject;

/// <summary>
/// IOCPCore class is a class that wraps I/O Completion Port (IOCP), 
/// <para>registers a file handle (IOCPObject) with IOCP, and processes completed tasks.</para>
/// </summary>
class IOCPCore : public enable_shared_from_this<IOCPCore>
{
public:
	static constexpr uint16 MAX_ENTRY_COUNT = 10;
	static constexpr BOOL ALERTABLE = FALSE;

	/// <summary>
	/// Constructor contains creating a new IOCP handle.
	/// </summary>
	IOCPCore();
	/// <summary>
	/// The handle will be closed.
	/// </summary>
	~IOCPCore();

	IOCPCore(const IOCPCore&) = delete;
	IOCPCore(IOCPCore&&) noexcept = delete;
	IOCPCore& operator=(const IOCPCore&) = delete;
	IOCPCore& operator=(IOCPCore&&) noexcept = delete;

public:
	/// <summary>
	/// Get IOCP Handle.
	/// </summary>
	/// <returns>IOCP Handle</returns>
	HANDLE GetHandle() const noexcept { return _iocpHandle; }

	/// <summary>
	/// Register a IOCPObject (file handle) to I/O completion port.
	/// </summary>
	/// <param name="iocpObject"></param>
	/// <returns>true if successful, false otherwise</returns>
	bool RegisterHandle(IOCPObjectSPtr iocpObject);

	/// <summary>
	/// Check Completion Status and call IOCPObject::Dispatch() of the object associated with the handle for the poped event.
	/// <para> It uses ::GetQueuedCompletionStatusEx.</para>
	/// </summary>
	/// <param name="dwTimeoutMilliseconds">Timeout (default is INFINITE)</param>
	/// <returns>true if getting queued event is successful, false otherwise</returns>
	bool ProcessQueuedCompletionStatusEx(DWORD dwTimeoutMilliseconds = INFINITE);
	
	/// <summary>
	/// Check Completion Status and call IOCPObject::Dispatch() of the object associated with the handle for the poped event.
	/// </summary>
	/// <param name="dwTimeoutMilliseconds">Timeout (default is INFINITE)</param>
	/// <returns>true if getting queued events is successful, false otherwise</returns>
	bool ProcessQueuedCompletionStatus(DWORD dwTimeoutMilliseconds = INFINITE);

private:
	HANDLE _iocpHandle = NULL; // Note: HANDLE is void*
};

NAMESPACE_CLOSE;