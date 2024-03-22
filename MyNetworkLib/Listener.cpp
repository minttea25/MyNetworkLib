#include "pch.h"
#include "Listener.h"

NetCore::Listener::Listener(SOCKADDR_IN& addr) : _addr(addr)
{
	WSADATA wsaData;
	// Returns 0 if successful.
	// Do not use ::WSAGetLastError() here to get error.
	int32 suc = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
	ASSERT_CRASH(suc == 0);

	_listenSocket = WSASocket(AF_INET, SOCK_STREAM,
		IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	ASSERT_CRASH(_listenSocket != 0);

	_initWSockFunctions();
}

NetCore::Listener::~Listener()
{
	int32 suc = ::WSACleanup();
	if (suc == SOCKET_ERROR)
	{
		ERR(suc, WSACleanup was failed.);
	}
	_listenSocket = INVALID_SOCKET;

	for (AcceptEvent* evt : _accepEvents) xxdelete(evt);

	ReleaseAllSessions();
}

bool NetCore::Listener::StartListen()
{
	bool suc = true;

	suc = SOCKET_ERROR != ::bind(_listenSocket, reinterpret_cast<PSOCKADDR>(&_addr), sizeof(SOCKADDR_IN));
	if (suc == false)
	{
		return false;
	}

	suc = SOCKET_ERROR != ::listen(_listenSocket, 100);
	if (suc == false)
	{
		return false;
	}

	for (int i = 0; i < MAX_ACCEPT_COUNT; ++i)
	{
		AcceptEvent* evt = xxnew<AcceptEvent>();
		evt->SetIOCPObjectRef(this);
		_accepEvents.push_back(evt);
		RegisterAccept(evt);
	}

	return true;
}

void NetCore::Listener::_initWSockFunctions()
{
	NOT_USE DWORD bytes = 0;
	GUID t = WSAID_ACCEPTEX;
	// WSAIoctl return 0 if successful.

	int32 suc = ::WSAIoctl(_listenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&t, sizeof(t),
		reinterpret_cast<LPVOID*>(&AcceptEx), sizeof(AcceptEx),
		OUT & bytes, NULL, NULL);
	if (suc != 0)
	{
		int32 errorCode = ::WSAGetLastError();
		ERR(errorCode, Failed WSAIoctl at AcceptEx.);
	}
	ASSERT_CRASH(suc == 0);
}

void NetCore::Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
	auto s = GetNewSession();

	acceptEvent->SetSessionRef(s);
	auto session = acceptEvent->GetSessionRef();
	auto acceptSocket = session->GetSocket();
	auto buf = session->GetRecvBuffer();
	auto ov = acceptEvent->overlapped();
	DWORD bytesReceived = 0;
	BOOL suc = AcceptEx(_listenSocket, acceptSocket,
		buf, 0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		OUT & bytesReceived, ov);

	if (suc == FALSE)
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			// Error
			ERR(errorCode, AcceptExFailed);
			
			RegisterAccept(acceptEvent);
		}
	}
}

void NetCore::Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
	Session* session = acceptEvent->GetSessionRef();

	int suc = ::setsockopt(session->GetSocket(), 
		SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, 
		reinterpret_cast<_byte*>(_listenSocket), sizeof(SOCKET));

	if (suc == SOCKET_ERROR)
	{
		RegisterAccept(acceptEvent);
		return;
	}

	SOCKADDR_IN sockAddr;
	int32 nameLen = sizeof(sockAddr);
	suc = ::getpeername(session->GetSocket(), OUT reinterpret_cast<sockaddr*>(&sockAddr), &nameLen);

	if (suc == SOCKET_ERROR)
	{
		RegisterAccept(acceptEvent);
		return;
	}

	session->SetConnected();
}

HANDLE NetCore::Listener::GetHandle()
{
	return reinterpret_cast<HANDLE>(_listenSocket);
}

void NetCore::Listener::Dispatch(IOCPEvent* event, int32 numberOfBytes)
{
	switch (event->GetEventType())
	{
	case EventType::Accept:
		ProcessAccept(static_cast<AcceptEvent*>(event));
		break;
	default:
		WARN(The event was not Accept.);
		break;
	}
}
