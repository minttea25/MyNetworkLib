#include "pch.h"
#include "Connector.h"

NetCore::Connector::Connector(SOCKADDR_IN& addr, std::function<Session*()> sessionFactory)
	: _connected(false),
	_addr(addr), _session_factory(sessionFactory)
{
	WSADATA wsaData;
	// Returns 0 if successful.
	// Do not use ::WSAGetLastError() here to get error.
	int32 suc = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
	ASSERT_CRASH(suc == 0);

	_initWSockFunctions();
}

NetCore::Connector::~Connector()
{
	if (_session != nullptr) xxdelete<Session>(_session);

	// Clear winsock
	::WSACleanup();
	_connectSocket = INVALID_SOCKET;
}

bool NetCore::Connector::Connect(IOCPCore& iocpCore)
{
	_session = _session_factory();

	// Create a async IO Socket using Overlapped model.
	_connectSocket = _session->GetSocket();

	// Check failed
	ASSERT_CRASH(_connectSocket != INVALID_SOCKET);

	SOCKADDR_IN bindAddr;
	memset(&bindAddr, 0, sizeof(bindAddr));
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	bindAddr.sin_port = ::htons(0); // any port

	// bind local
	if (SOCKET_ERROR == ::bind(_connectSocket, reinterpret_cast<PSOCKADDR>(&bindAddr), sizeof(bindAddr)))
	{
		auto errorCode = ::WSAGetLastError();
		ERR(errorCode, Failed to bind.);
		return false;
	}

	// register handle
	ASSERT_CRASH(iocpCore.RegisterIOCP(this) == true);

	_connectEvent.Clear();
	_connectEvent.SetIOCPObjectRef(this);

	/*bool keepalive = true;
	bool s = SOCKET_ERROR != ::setsockopt(_connectSocket, SOL_SOCKET,
		SO_KEEPALIVE, reinterpret_cast<char*>(&keepalive), sizeof(bool));
	ASSERT_CRASH(s);*/

	DWORD bytesSent = 0; // OUT
	auto suc = ConnectEx(_connectSocket, reinterpret_cast<PSOCKADDR>(&_addr),
		sizeof(SOCKADDR), nullptr, 0, OUT & bytesSent, &_connectEvent);

	if (suc == FALSE)
	{
		// Check if the socket is pending-status
		auto errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			// Error
			ERR(errorCode, Error at ConnectEx);
			_connectEvent.SetIOCPObjectRef(nullptr);
			return false;
		}
	}

	return true;
}

void NetCore::Connector::_initWSockFunctions()
{
	SOCKET dummy = ::WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	ASSERT_CRASH(dummy != INVALID_SOCKET);

	NOT_USE DWORD bytes = 0;
	GUID t = WSAID_CONNECTEX;
	// WSAIoctl return 0 if successful.
	int32 suc = ::WSAIoctl(dummy, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&t, sizeof(t),
		reinterpret_cast<LPVOID*>(&ConnectEx), sizeof(ConnectEx),
		OUT &bytes, NULL, NULL);

	if (suc != 0)
	{
		int32 errorCode = ::WSAGetLastError();
		ERR(errorCode, Failed WSAIoctl at ConnectEx.);
	}

	ASSERT_CRASH(suc == 0);
	::closesocket(dummy);
	dummy = (~0);
}

void NetCore::Connector::_processConnect()
{
	_connectEvent.SetIOCPObjectRef(nullptr);
	ASSERT_CRASH(_session != nullptr);
	_session->SetConnected();
}

void NetCore::Connector::Dispatch(IOCPEvent* event, int32 numberOfBytes)
{
	switch (event->GetEventType())
	{
	case EventType::Connect:
		event->SetIOCPObjectRef(_session);
		_processConnect();
		break;
	default:
		WARN(The event was not Connect.);
		break;
	}
}

HANDLE NetCore::Connector::GetHandle()
{
	return reinterpret_cast<HANDLE>(_connectSocket);
}

