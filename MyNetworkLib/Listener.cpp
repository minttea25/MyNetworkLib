#include "pch.h"
#include "Listener.h"

NetCore::Listener::Listener(SOCKADDR_IN& addr, std::function<Session* ()> session_factory, IOCPCore& core) 
	: _addr(addr), _session_factory(session_factory), _core(core)
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
	ASSERT_CRASH(_core.RegisterIOCP(this));

	bool reuse_address = true;
	suc = SOCKET_ERROR != ::setsockopt(_listenSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&reuse_address), sizeof(bool));
	ASSERT_CRASH(suc);



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
	SOCKET dummy = ::WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	ASSERT_CRASH(dummy != INVALID_SOCKET);
	NOT_USE DWORD bytes = 0;
	GUID t = WSAID_ACCEPTEX;
	// WSAIoctl return 0 if successful.

	int32 suc = ::WSAIoctl(dummy, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&t, sizeof(t),
		reinterpret_cast<LPVOID*>(&AcceptEx), sizeof(AcceptEx),
		OUT & bytes, NULL, NULL);
	if (suc != 0)
	{
		int32 errorCode = ::WSAGetLastError();
		ERR(errorCode, Failed WSAIoctl at AcceptEx.);
	}
	ASSERT_CRASH(suc == 0);
	::closesocket(dummy);
	dummy = (~0);
}

void NetCore::Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
	auto s = _get_new_session();

	acceptEvent->Clear();
	acceptEvent->SetSessionRef(s);

	auto session = acceptEvent->GetSessionRef();
	DWORD bytesReceived = 0;
	BOOL suc = AcceptEx(_listenSocket, session->GetSocket(),
		session->GetRecvBuffer(), 0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		OUT & bytesReceived, acceptEvent->overlapped());

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
		reinterpret_cast<_byte*>(&_listenSocket), sizeof(SOCKET));

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

	RegisterAccept(acceptEvent);
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
