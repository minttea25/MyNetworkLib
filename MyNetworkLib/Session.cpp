#include "pch.h"
#include "Session.h"

NetCore::Session::Session()
{
	_socket = SocketUtils::CreateSocket();
}

NetCore::Session::~Session()
{
	DESTRUCTOR(Session);

	SocketUtils::Close(_socket);
}

void NetCore::Session::SetConnected(const ServiceSPtr service, Socket connectedSocket)
{
	_service = service;

	if (connectedSocket != INVALID_SOCKET) _set_socket(connectedSocket);

	_connected.store(true);

	// Call virtual method
	OnConnected();

	// Start to recive data
	RegisterRecv();
}

void NetCore::Session::Send(const _byte* buffer)
{
	if (_connected == false)
	{
		// Stop receiving
		WARN(Session is already disconnected.);
	}

	bool sendFlag = false;

	const uint32 size = static_cast<uint32>(strlen(buffer));

	/// TLS
	auto pos = TLS_SendBuffer->Write(size);
	std::copy(buffer, buffer + size, pos);
	// ------------
	{
		WRITE_LOCK(send);
		auto seg = NetCore::make_shared<SendBufferSegment>(TLS_SendBuffer->shared_from_this(), pos, size);
		_sendQueue.push(seg);

		if (_sending.exchange(true) == false)
		{
			sendFlag = true;
		}

	}

	if (sendFlag) RegisterSend();
}

bool NetCore::Session::Disconnect()
{
	if (IsConnected() == false) return false;

	return RegisterDisconnect();
}

void NetCore::Session::_set_socket(Socket connectedSocket)
{
	if (ErrorHandler::CheckError(_service->GetServiceType() == ServiceType::Client, Errors::APP_SESSION_SET_SOCKET_ALLOWED_ONLY_IN_CONNECTOR))
	{
		return;
	}

	_socket = connectedSocket;
}


void NetCore::Session::_disconnect(uint16 errorCode)
{
#ifdef TEST
	SHOW(DisconnectError:, errorCode);
#endif // TEST

	if (IsConnected() == false) return;

	RegisterDisconnect();
}

void NetCore::Session::Process(IOCPEvent * overlappedEvent, DWORD numberOfBytesTransferred)
{
	EventType type = overlappedEvent->GetEventType();
	SHOW(EventType, (int)type);
	switch (type)
	{
	case EventType::Recv:
		ProcessRecv(numberOfBytesTransferred);
		break;
	case EventType::Send:
		ProcessSend(numberOfBytesTransferred);
		break;
	case EventType::Disconnect:
		ProcessDisconnect();
		break;
	default:
		WARN(Received event type was not recv/send/disconnect.);
		break;
	}
}

HANDLE NetCore::Session::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void NetCore::Session::RegisterSend()
{
	// Note: Because _connected is atomic variable, reading just _connected is sames as reading _connected.load().
	if (_connected == false)
	{
		// Stop receiving
		WARN(Session is already disconnected.);
		return;
	}

	_sendEvent.Clear();
	_sendEvent.SetIOCPObjectSPtr(shared_from_this());

	{
		WRITE_LOCK(send);

		// get msgs from queue
		while (_sendQueue.empty() == false)
		{
			std::shared_ptr<SendBufferSegment> segment = _sendQueue.front();
			_sendQueue.pop();

			_sendEvent._segments.push_back(segment);
		}
	}

	Vector<WSABUF> wsaSendBuffers;
	wsaSendBuffers.reserve(_sendEvent._segments.size());
	for (auto buffer : _sendEvent._segments)
	{
		WSABUF wsaBuf{};
		wsaBuf.buf = reinterpret_cast<char*>(buffer->GetBufferSegment());
		wsaBuf.len = static_cast<ULONG>(buffer->GetSize());
		wsaSendBuffers.push_back(wsaBuf);
	}

	DWORD numberOfBytesSent = 0;
	int32 res = ::WSASend(_socket, wsaSendBuffers.data(),
		static_cast<DWORD>(wsaSendBuffers.size()),
		OUT & numberOfBytesSent, 0, &_sendEvent, NULL);

	if (ErrorHandler::WSACheckErrorExceptPending(res != SOCKET_ERROR, Errors::WSA_SEND_FAILED) != Errors::NONE)
	{
		// TODO
		_sendEvent.ReleaseIOCPObjectSPtr(); // ?
	}
	_sendEvent._segments.clear();
	_sending.store(false);
}

void NetCore::Session::ProcessSend(const int32 numberOfBytesSent)
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

void NetCore::Session::RegisterRecv()
{
	// Note: Because _connected is atomic variable, reading just _connected is sames as reading _connected.load().
	if (_connected == false)
	{
		// Stop receiving
		WARN(Session is already disconnected.);
		return;
	}

	_recvEvent.Clear();
	_recvEvent.SetIOCPObjectSPtr(shared_from_this());

	// TEMP
	WSABUF recvBuffer {};
	recvBuffer.buf = reinterpret_cast<_byte*>(GetRecvBuffer());
	recvBuffer.len = MAX_BUFFER_SIZE;

	DWORD numberOfBytesRecvd = 0; // OUT
	DWORD flags = 0;
	int32 res = ::WSARecv(_socket, &recvBuffer, 1,
		OUT & numberOfBytesRecvd, OUT & flags, &_recvEvent, NULL);

	if (ErrorHandler::WSACheckErrorExceptPending(res != SOCKET_ERROR, WSA_RECV_FAILED) != Errors::NONE)
	{

	}
}

void NetCore::Session::ProcessRecv(const uint32 numberOfBytesRecvd)
{
	if (numberOfBytesRecvd == 0)
	{
		// Error
		_disconnect(DisconnectError::RECV_0);
		return;
	}

	int32 processLen = OnRecv(_recvBuffer, numberOfBytesRecvd);

	if (processLen == 0)
	{
		// Error
		_disconnect(DisconnectError::PROCESS_LEN_0);
		return;
	}

	// Reister again.
	RegisterRecv();
}

bool NetCore::Session::RegisterDisconnect()
{
	_disconnectEvent.Clear();
	_disconnectEvent.SetIOCPObjectSPtr(shared_from_this());

	BOOL suc = SocketUtils::DisconnectEx(_socket, &_disconnectEvent, TF_REUSE_SOCKET, 0);
	if (ErrorHandler::WSACheckErrorExceptPending(suc, Errors::WSA_DISCONNECTEX_FAILED) == false)
	{
		return false;
	}
	return true;
}

void NetCore::Session::ProcessDisconnect()
{
	_disconnectEvent.ReleaseIOCPObjectSPtr();

	_connected.store(false);

	// Note: ReleaseSession will return false if this session is already removed in service.
	// It can occur when the service is stopped.
	_service->ReleaseSession(static_pointer_cast<Session>(shared_from_this()));

	_service = nullptr;

	OnDisconnected();
}
