#pragma once

NAMESPACE_OPEN(NetCore);

/// <summary>
/// It is a class that contains IOCP handle and how to process the completed tasks.
/// </summary>
ABSTRACT class IOCPObject : public enable_shared_from_this<IOCPObject>
{
public:
	virtual ~IOCPObject() {}
	/// <summary>
	/// Get IOCP handle.
	/// </summary>
	/// <returns>IOCP Handle</returns>
	virtual HANDLE GetHandle() abstract;
public:
	/// <summary>
	/// [Pure] Process the completed task (IOCPEvent).
	/// </summary>
	/// <param name="overlappedEvent">Overlapped Event (IOCPEvent)</param>
	/// <param name="numberOfBytesTransferred">Number of bytes transferred.</param>
	virtual void Dispatch(IOCPEvent * overlappedEvent, DWORD numberOfBytesTransferred) abstract;
private:

};

NAMESPACE_CLOSE;
