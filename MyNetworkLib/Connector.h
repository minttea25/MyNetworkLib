#pragma once

#include <WinSock2.h>
#include <MSWSock.h>
#include <Windows.h>

#include "Session.h"
#include "IOCPEvent.h"

NAMESPACE_OPEN(NetCore);

static LPFN_CONNECTEX ConnectEx = nullptr;




class Session;
struct ConnectEvent;

class Connector : public IOCPObject
{
public:
	Connector(SOCKADDR_IN& addr, std::function<Session*()> sessionFactory);
	~Connector();

	bool Connect(IOCPCore& core);
public:
	bool connected() const { return _connected.load(); }
private:
	void _initWSockFunctions();
	void _processConnect();

	// Inherited via IOCPObject
	void Dispatch(IOCPEvent* event, int32 numberOfBytes) override;
	HANDLE GetHandle() override;
private:
	SOCKET _connectSocket = INVALID_SOCKET;
	SOCKADDR_IN _addr;
	ConnectEvent _connectEvent;
	Atomic<bool> _connected = false;

	Session* _session;
	std::function<Session*()> _session_factory ;
};

NAMESPACE_CLOSE;

