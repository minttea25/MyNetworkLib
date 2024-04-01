#include "pch.h"
#include "Listener.h"

NetCore::Listener::Listener(SOCKADDR_IN& addr, std::function<SessionSPtr()> session_factory, IOCPCore& core) 
	: _addr(addr), _session_factory(session_factory), _core(core)
{
	_listenSocket = SocketUtils::CreateSocket();
}

NetCore::Listener::~Listener()
{
	SocketUtils::Close(_listenSocket);

	for (AcceptEvent* evt : _accepEvents) xxdelete(evt);

	ReleaseAllSessions();
}

bool NetCore::Listener::StartListen(const int32 backlog)
{
	if (_core.RegisterHandle(shared_from_this()) == false) return false;

	if (SocketUtils::SetReuseAddress(_listenSocket, true) == false) return false;
	if (SocketUtils::Bind(_listenSocket, &_addr) == false) return false;
	if (SocketUtils::Listen(_listenSocket, backlog) == false) return false;

	for (int i = 0; i < MAX_ACCEPT_COUNT; ++i)
	{
		AcceptEvent* evt = xxnew<AcceptEvent>();
		evt->SetIOCPObjectRef(shared_from_this());
		_accepEvents.push_back(evt);
		RegisterAccept(evt);
	}

	return true;
}

void NetCore::Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
	auto s = _session_factory();
	_core.RegisterHandle(s);
	sessions.push_back(s);

	acceptEvent->Clear();
	acceptEvent->SetSessionRef(s);

	auto session = acceptEvent->GetSessionRef();
	NOT_USE DWORD bytesReceived = 0;
	BOOL suc = SocketUtils::AcceptEx(_listenSocket, session->GetSocket(),
		session->GetRecvBuffer(), 0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		OUT & bytesReceived, acceptEvent->overlapped());

	if (ErrorHandler::WSACheckErrorExceptPending(suc, WSA_ACCEPTEX_FAILED) != Errors::NONE)
	{
		RegisterAccept(acceptEvent);
	}

	//if (suc == FALSE)
	//{
	//	const int32 errorCode = ::WSAGetLastError();
	//	if (errorCode != WSA_IO_PENDING)
	//	{
	//		// Error
	//		ERR_CODE(errorCode, AcceptExFailed);
	//		
	//		RegisterAccept(acceptEvent);
	//	}
	//}
}

void NetCore::Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
	SessionSPtr session = acceptEvent->GetSessionRef();

	if (SocketUtils::SetUpdateAcceptSocket(session->GetSocket(), _listenSocket) == false)
	{
		RegisterAccept(acceptEvent);
		return;
	}

	SOCKADDR_IN sockAddr{};
	int32 nameLen = sizeof(sockAddr);
	bool suc = ::getpeername(session->GetSocket(), OUT reinterpret_cast<sockaddr*>(&sockAddr), &nameLen);

	if (ErrorHandler::WSACheckSocketError(suc, Errors::WSA_GET_PEER_NAME_FAILED) == false)
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

void NetCore::Listener::Process(IOCPEvent * overlappedEvent, DWORD numberOfBytesTransferred)
{
	switch (overlappedEvent->GetEventType())
	{
	case EventType::Accept:
		ProcessAccept(static_cast<AcceptEvent*>(overlappedEvent));
		break;
	default:
		WARN(The event was not Accept.);
		break;
	}
}
