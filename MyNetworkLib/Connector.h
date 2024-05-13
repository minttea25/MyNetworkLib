#pragma once

#include "Session.h"
#include "IOCPEvent.h"

NAMESPACE_OPEN(NetCore);

class Session;
class IOCPCore;
struct ConnectEvent;

/// <summary>
/// Connector is a class to use connecting to other end point.
/// </summary>
class Connector : public IOCPObject
{
public:
	Connector(ServiceSPtr clientService);
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
	void Dispatch(IOCPEvent* overlappedEvent, DWORD numberOfBytesTransferred) override;
	/// <summary>
	/// Get socket handle;
	/// </summary>
	/// <returns>socket handle</returns>
	HANDLE GetHandle() override;
private:
	ServiceSPtr _clientService;
	SessionWPtr _session;
	SOCKET _connectSocket = INVALID_SOCKET;
	ConnectEvent _connectEvent;
	Atomic<bool> _connected = false;
};

NAMESPACE_CLOSE;

