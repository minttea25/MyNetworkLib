#include "pch.h"
#include "Connector.h"

NetCore::Connector::Connector(SOCKADDR_IN& addr, Session& session)
	: _connected(false), _connectSocket(INVALID_SOCKET),
	_addr(addr), _session(&session)
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
	// Clear winsock
	::WSACleanup();
	_connectSocket = INVALID_SOCKET;
}

bool NetCore::Connector::Connect()
{
	// Create a async IO Socket using Overlapped model.
	_connectSocket = ::WSASocket(AF_INET, SOCK_STREAM,
		IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

	// Check failed
	ASSERT_CRASH(_connectSocket != INVALID_SOCKET);

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
			ERR(errorCode, SocketError);
			return false;
		}
	}

	return true;
}

void NetCore::Connector::_initWSockFunctions()
{
	NOT_USE DWORD bytes = 0;
	GUID t = WSAID_CONNECTEX;
	// WSAIoctl return 0 if successful.
	int32 suc = ::WSAIoctl(_connectSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&t, sizeof(t),
		reinterpret_cast<LPVOID*>(ConnectEx), sizeof(ConnectEx),
		OUT &bytes, NULL, NULL);

	if (suc != 0)
	{
		int32 errorCode = ::WSAGetLastError();
		ERR(errorCode, Failed WSAIoctl at ConnectEx.);
	}

	ASSERT_CRASH(suc == 0);
}

void NetCore::Connector::_processConnect()
{
	_session->SetConnected();
}

void NetCore::Connector::Dispatch(IOCPEvent* event, int32 numberOfBytes)
{
	switch (event->GetEventType())
	{
	case EventType::Connect:
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

