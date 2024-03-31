#include "pch.h"
#include "Connector.h"

NetCore::Connector::Connector(IOCPCore* core, SOCKADDR_IN& addr, std::function<SessionSPtr()> sessionFactory)
	: _connected(false), _core(core),
	_addr(addr), _session_factory(sessionFactory)
{
	_connectSocket = SocketUtils::CreateSocket();
	_session = _session_factory();
	_connectSocket = _session->GetSocket();
}

NetCore::Connector::~Connector()
{
	// The socket is referrenced from Session.
	// SocketUtils::Close(_connectSocket);
}

bool NetCore::Connector::Connect()
{
	if (_connected.load() == true) return false;

	if (_core->RegisterHandle(shared_from_this()) == false) return false;

	if (SocketUtils::SetReuseAddress(_connectSocket, true) == false) return false;
	// For ConnectEx fn
	if (SocketUtils::BindAnyAddress(_connectSocket) == false) return false;

	// Init event
	_connectEvent.Clear();
	_connectEvent.SetIOCPObjectRef(shared_from_this());

	NOT_USE DWORD bytesSent = 0;
	BOOL suc = SocketUtils::ConnectEx(_connectSocket, reinterpret_cast<PSOCKADDR>(&_addr),
		sizeof(SOCKADDR), nullptr, 0, OUT & bytesSent, &_connectEvent);

	// Note: WSACheckErrorExceptPending return 0 if no error or WSA_IO_PENDING
	if (int32 errCode = ErrorHandler::WSACheckErrorExceptPending(suc, WSA_CONNECTEX_FAILED) != Errors::NONE)
	{
		_connectEvent.SetIOCPObjectRef(nullptr);
		return false;
	}

	//if (suc == FALSE)
	//{
	//	// Check if the socket is pending-status
	//	auto errorCode = ::WSAGetLastError();
	//	if (errorCode != WSA_IO_PENDING)
	//	{
	//		// Error
	//		ERR_CODE(errorCode, Error at ConnectEx);
	//		_connectEvent.SetIOCPObjectRef(nullptr);
	//		return false;
	//	}
	//}

	return true;
}


void NetCore::Connector::_processConnect()
{
	// Release pointer
	_connectEvent.SetIOCPObjectRef(nullptr);
	_connected.store(true);
	_session->SetConnected();
}

void NetCore::Connector::Process(IOCPEvent * overlappedEvent, DWORD numberOfBytesTransferred)
{
	switch (overlappedEvent->GetEventType())
	{
	case EventType::Connect:
		// Change iocp object
		overlappedEvent->SetIOCPObjectRef(_session);
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

