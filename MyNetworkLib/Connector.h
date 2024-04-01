#pragma once

#include "Session.h"
#include "IOCPEvent.h"

NAMESPACE_OPEN(NetCore);

class Session;
struct ConnectEvent;

/// <summary>
/// Connector is a class to use connecting to other end point.
/// </summary>
class Connector : public IOCPObject
{
public:
	Connector(IOCPCore* core, SOCKADDR_IN& addr, std::function<SessionSPtr()> sessionFactory);
	~Connector();

	/// <summary>
	/// Try to connect to address.
	/// </summary>
	/// <returns>true if trying to connect is successful, false otherwise.</returns>
	bool Connect();
public:
	bool connected() const { return _connected.load(); }
private:
	/// <summary>
	/// Called when the connection 
	/// </summary>
	void _processConnect();

	// Inherited via IOCPObject
	void Process(IOCPEvent* overlappedEvent, DWORD numberOfBytesTransferred) override;
	/// <summary>
	/// Get socket handle;
	/// </summary>
	/// <returns>socket handle</returns>
	HANDLE GetHandle() override;
private:
	SOCKET _connectSocket = INVALID_SOCKET;
	SOCKADDR_IN _addr;
	ConnectEvent _connectEvent;
	Atomic<bool> _connected = false;

	SessionSPtr _session = nullptr;
	std::function<SessionSPtr()> _session_factory ;
	IOCPCore* _core;
};

NAMESPACE_CLOSE;

