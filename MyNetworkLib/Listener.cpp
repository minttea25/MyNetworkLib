#include "pch.h"
#include "Listener.h"

NetCore::Listener::Listener(ServiceSPtr serverService)
	: _serverService(serverService)
{
	ErrorHandler::AssertCrash(serverService->GetServiceType() == ServiceType::Server, APP_LISTENER_SERVICE_WAS_NOT_SERVER);
	
	_listenSocket = SocketUtils::CreateSocket();
}

NetCore::Listener::~Listener()
{
	DESTRUCTOR(Listener);

	SocketUtils::Close(_listenSocket);

	for (AcceptEvent* evt : _acceptEvents) xxdelete(evt);
}

bool NetCore::Listener::StartListen(const int32 backlog)
{
	if (_serverService->_iocpCore->RegisterHandle(shared_from_this()) == false) return false;

	if (SocketUtils::SetLinger(_listenSocket, 0, 0) == false) return false;
	if (SocketUtils::SetReuseAddress(_listenSocket, true) == false) return false;
	if (SocketUtils::Bind(_listenSocket, &(_serverService->_addr)) == false) return false;
	if (SocketUtils::Listen(_listenSocket, backlog) == false) return false;

	for (int i = 0; i < MAX_ACCEPT_COUNT; ++i)
	{
		AcceptEvent* evt = xxnew<AcceptEvent>();
		evt->SetIOCPObjectSPtr(shared_from_this());
		_acceptEvents.push_back(evt);
		RegisterAccept(evt);
	}

	return true;
}

void NetCore::Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
	auto s = _serverService->AddNewSession();

	acceptEvent->Clear();
	acceptEvent->SetSessionRef(s);

	auto session = acceptEvent->GetSessionRef();
	IGNORED DWORD bytesReceived = 0;
	BOOL suc = SocketUtils::AcceptEx(_listenSocket, session->GetSocket(),
		session->_recvBuffer.WritePos(), 0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		OUT & bytesReceived, acceptEvent->overlapped());

	if (ErrorHandler::WSACheckErrorExceptPending(suc, WSA_ACCEPTEX_FAILED) != Errors::NONE)
	{
		RegisterAccept(acceptEvent);
	}
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

	
	session->SetConnected(_serverService);

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
