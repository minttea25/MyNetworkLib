#include "pch.h"
#include "Connector.h"

NetCore::Connector::Connector(ServiceSPtr clientService)
	: _connected(false), _clientService(clientService)
{
	// Note: Service must be a client service.
	ErrorHandler::AssertCrash(
		clientService->GetServiceType() == ServiceType::Client, 
		Errors::APP_CONNECTOR_SERVICE_WAS_NOT_CLIENT);

	_connectSocket = SocketUtils::CreateSocket();
}

NetCore::Connector::~Connector()
{
	DESTRUCTOR(Connector);

	// The socket is referrenced from Session.
	// SocketUtils::Close(_connectSocket);
}

bool NetCore::Connector::Connect()
{
	if (_connected.load() == true) return false;

	if (_clientService->_iocpCore->RegisterHandle(shared_from_this()) == false) return false;

	if (SocketUtils::SetReuseAddress(_connectSocket, true) == false) return false;
	// For ConnectEx fn
	if (SocketUtils::BindAnyAddress(_connectSocket) == false) return false;

	// Init event
	_connectEvent.Clear();
	_connectEvent.SetIOCPObjectSPtr(shared_from_this());

	IGNORED DWORD bytesSent = 0;
	BOOL suc = SocketUtils::ConnectEx(
		_connectSocket, 
		reinterpret_cast<PSOCKADDR>(&(_clientService->_addr)), sizeof(SOCKADDR), 
		nullptr, 0, OUT & bytesSent, 
		&_connectEvent);

	// Note: WSACheckErrorExceptPending return 0 if no error or WSA_IO_PENDING
	if (int32 errCode = ErrorHandler::WSACheckErrorExceptPending(suc, WSA_CONNECTEX_FAILED) != Errors::NONE)
	{
		_connectEvent.SetIOCPObjectSPtr(nullptr);
		return false;
	}

	return true;
}


void NetCore::Connector::_processConnect()
{
	// Release pointer
	_connectEvent.ReleaseIOCPObjectSPtr();
	_connected.store(true);
	_session.lock()->_set_connected(_clientService, _connectSocket);
}

void NetCore::Connector::Dispatch(IOCPEvent * overlappedEvent, DWORD numberOfBytesTransferred)
{
	switch (overlappedEvent->GetEventType())
	{
	case EventType::Connect:
		// Create a session and change iocp object to it.
		_session = _clientService->AddNewSession();
		overlappedEvent->SetIOCPObjectSPtr(_session.lock());
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

