#include "pch.h"
#include "Session.h"

NetCore::Session::Session()
	:_connected(false), _recvBuffer(RECV_BUFFER_SIZE)
{
	_sessionId = s_sid.fetch_add(1);

	_socket = SocketUtils::CreateSocket();
}

NetCore::Session::~Session()
{
	DESTRUCTOR(Session);

	SocketUtils::Close(_socket);
}

void NetCore::Session::_set_connected(const ServiceSPtr service, Socket connectedSocket)
{
	_service = service;

	if (connectedSocket != INVALID_SOCKET) _set_socket(connectedSocket);

	_connected.store(true);

	// Call virtual method
	OnConnected();

	// Start to recive data
	_register_recv();
}

void NetCore::Session::_send(Vector<WSABUF>& buffers)
{
	if (buffers.size() == 0) return;

	bool sendFlag = false;
	{
		WRITE_LOCK(send);

		_sendQueue.reserve(buffers.size());
		std::move(buffers.begin(), buffers.end(), std::back_inserter(_sendQueue));

		if (_sending.exchange(true) == false)
		{
			sendFlag = true;
		}
	}
	if (sendFlag) _register_send();
}

bool NetCore::Session::Disconnect()
{
	if (_connected.exchange(false) == false) return false;

	return _register_disconnect();
}

void NetCore::Session::_set_socket(Socket connectedSocket)
{
	if (ErrorHandler::CheckError(_service->GetServiceType() == ServiceType::Client, Errors::APP_SESSION_SET_SOCKET_ALLOWED_ONLY_IN_CONNECTOR))
	{
		return;
	}

	_socket = connectedSocket;
}


void NetCore::Session::_disconnect(const uint16 errorCode)
{
	if (IsConnected() == false) return;

	__NETCORE_LOG_VALUE(Disconnected by Error : , errorCode);

	_register_disconnect();
}

void NetCore::Session::_clear()
{
	// Note: ReleaseSession will return false if this session is already removed in service.
	// It can occur when the service is stopped.
	if (_service->ReleaseSession(static_pointer_cast<Session>(shared_from_this())) == false)
	{
		__NETCORE_LOG_INFO(ReelaseSession in service is failed.);
	}

	_service = nullptr;
	_sendEvent.ReleaseIOCPObjectSPtr();
	_recvEvent.ReleaseIOCPObjectSPtr();
	_disconnectEvent.ReleaseIOCPObjectSPtr();
	_sendQueue.clear();
	_connected = false;
	_socket = INVALID_SOCKET;
}

void NetCore::Session::Dispatch(IOCPEvent* overlappedEvent, DWORD numberOfBytesTransferred)
{
	EventType type = overlappedEvent->GetEventType();
	switch (type)
	{
	case EventType::Recv:
		_process_recv(numberOfBytesTransferred);
		break;
	case EventType::Send:
		_process_send(numberOfBytesTransferred);
		break;
	case EventType::Disconnect:
		_process_disconnect();
		break;
	default:
		__NETCORE_LOG_ERROR(Received event type was not recv / send / disconnect at Session.);
		break;
	}
}

HANDLE NetCore::Session::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void NetCore::Session::_register_send()
{
	// Note: Because _connected is atomic variable, reading just _connected is sames as reading _connected.load().
	if (_connected == false)
	{
		// Stop receiving
		__NETCORE_LOG_WARNING(Session is already disconnected.);
		_disconnect(DisconnectError::ALREADY_DISCONNECTED);
		return;
	}

	_sendEvent.Clear();
	_sendEvent.SetIOCPObjectSPtr(shared_from_this());

	{
		WRITE_LOCK(send);

		std::move(_sendQueue.begin(), _sendQueue.end(), back_inserter(_sendEvent._segments));
		_sendQueue.clear();
	}

	// CHECK
	DWORD numberOfBytesSent = 0;
	int32 res = ::WSASend(_socket, _sendEvent._segments.data(),
		static_cast<DWORD>(_sendEvent._segments.size()),
		OUT & numberOfBytesSent, 0, &_sendEvent, NULL);

	OUT int32 wsaLastError = 0;
	if (ErrorHandler::WSACheckErrorExceptPending2(res != SOCKET_ERROR, OUT wsaLastError) == false)
	{
		__NETCORE_CODE_ERROR(Errors::WSA_SEND_FAILED);
		_sendEvent.ReleaseIOCPObjectSPtr();
	}

	_sendEvent._segments.clear();
	_sending.store(false);
}

void NetCore::Session::_process_send(const int32 numberOfBytesSent)
{
	// clear to reuse event
	_sendEvent.ReleaseIOCPObjectSPtr();

	if (numberOfBytesSent == 0)
	{
		// Error
		_disconnect(DisconnectError::SENT_0);
		return;
	}

	OnSend(numberOfBytesSent);
}

void NetCore::Session::_register_recv()
{
	// Note: Because _connected is atomic variable, reading just _connected is sames as reading _connected.load().
	if (_connected == false)
	{
		// Stop receiving
		__NETCORE_LOG_WARNING(Session is already disconnected.);
		_disconnect(DisconnectError::ALREADY_DISCONNECTED);
		return;
	}

	_recvEvent.Clear();
	_recvEvent.SetIOCPObjectSPtr(shared_from_this());

	WSABUF recvBuffer
	{
		_recvBuffer.FreeSize(), // len
		_recvBuffer.WritePos() // buf
	};

	OUT DWORD numberOfBytesRecvd = 0; // OUT
	OUT DWORD flags = 0;
	int32 res = ::WSARecv(_socket, &recvBuffer, 1,
		OUT & numberOfBytesRecvd, OUT & flags, &_recvEvent, NULL);

	OUT int32 wsaError = 0;
	if (ErrorHandler::WSACheckErrorExceptPending2(res != SOCKET_ERROR, OUT wsaError) == false)
	{
		if (wsaError == WSAECONNRESET)
		{
			// disconnected
			__NETCORE_LOG_WSA_ERROR(wsaError);
		}
		else
		{
			__NETCORE_LOG_WSA_ERROR(wsaError);
		}
	}
	else if (res == 0)
	{
		// Connection closed gracefully
		std::cout << "o" << std::endl;
	}
}

void NetCore::Session::_process_recv(const uint32 numberOfBytesRecvd)
{
	if (numberOfBytesRecvd == 0)
	{
		// Error
		_disconnect(DisconnectError::RECV_0);
		return;
	}

	int32 processLen = OnRecv(_recvBuffer.DataPos(), numberOfBytesRecvd);

	if (processLen == 0)
	{
		// Error
		_disconnect(DisconnectError::PROCESS_LEN_0);
		return;
	}

	// Reister again.
	_register_recv();
}

bool NetCore::Session::_register_disconnect()
{
	_disconnectEvent.Clear();
	_disconnectEvent.SetIOCPObjectSPtr(shared_from_this());

	BOOL suc = SocketUtils::DisconnectEx(_socket, &_disconnectEvent, TF_REUSE_SOCKET, 0);

	OUT int32 wsaLastError = 0;
	if (ErrorHandler::WSACheckErrorExceptPending2(suc, OUT wsaLastError) == false)
	{
		__NETCORE_CODE_ERROR(Errors::WSA_DISCONNECTEX_FAILED);
		return false;
	}
	return true;
}

void NetCore::Session::_process_disconnect()
{
	_clear();

	OnDisconnected();
}

NetCore::Atomic<NetCore::uint32> NetCore::Session::s_sid = 1;
