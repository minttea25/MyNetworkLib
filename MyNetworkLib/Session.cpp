#include "pch.h"
#include "Session.h"

NetCore::Session::Session()
{
	_socket = SocketUtils::CreateSocket();
}

NetCore::Session::~Session()
{
	SocketUtils::Close(_socket);
}

void NetCore::Session::SetConnected()
{


	_connected.store(true);

	// Call virtual method
	OnConnected();

	// Start to recive data
	RegisterRecv();
}

bool NetCore::Session::Send(const _byte* buffer)
{
	if (_connected== false)
	{
		// Stop receiving
		WARN(Session is already disconnected.);
		return false;
	}

	std::copy(buffer, buffer + strlen(buffer) + 1, _sendBuffer);

	RegisterSend();

	return true;
}

void NetCore::Session::Disconnect(uint16 errorCode = DisconnectError::NONE)
{
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
	_sendEvent.SetIOCPObjectRef(shared_from_this());

	//{
	//	_sendLock.lock();

	//	// TODO : processing with queue

	//	_sendLock.unlock();
	//}


	WSABUF sendBuffer{};
	sendBuffer.buf = reinterpret_cast<_byte*>(GetSendBuffer());
	sendBuffer.len = MAX_BUFFER_SIZE;

	DWORD numberOfBytesSent = 0;
	int32 res = ::WSASend(_socket, &sendBuffer, 1,
		OUT & numberOfBytesSent, 0, &_sendEvent, NULL);

	if (ErrorHandler::WSACheckErrorExceptPending(res != SOCKET_ERROR, Errors::WSA_SEND_FAILED) != Errors::NONE)
	{
		// TODO
	}

	//if (res == SOCKET_ERROR)
	//{
	//	// Check pending
	//	int32 errorCode = ::WSAGetLastError();
	//	if (errorCode != WSA_IO_PENDING)
	//	{
	//		// Error
	//		ERR_CODE(errorCode, "WSASend Error");
	//	}
	//}
}

void NetCore::Session::ProcessSend(const int32 numberOfBytesSent)
{
	// clear to reuse event
	_sendEvent.SetIOCPObjectRef(nullptr);

	if (numberOfBytesSent == 0)
	{
		// Error
		Disconnect(DisconnectError::SENT_0);
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
	_recvEvent.SetIOCPObjectRef(shared_from_this());

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
		// TODO
	}

	//if (res == SOCKET_ERROR)
	//{
	//	// Check pending
	//	int32 errorCode = ::WSAGetLastError();
	//	if (errorCode != WSA_IO_PENDING)
	//	{
	//		// Error
	//		ERR_CODE(errorCode, "WSARecv Error");

	//	}
	//}
}

void NetCore::Session::ProcessRecv(const uint32 numberOfBytesRecvd)
{
	if (numberOfBytesRecvd == 0)
	{
		// Error
		Disconnect(DisconnectError::RECV_0);
		return;
	}

	int32 processLen = OnRecv(_recvBuffer, numberOfBytesRecvd);

	if (processLen == 0)
	{
		// Error
		Disconnect(DisconnectError::PROCESS_LEN_0);
		return;
	}

	// Reister again.
	RegisterRecv();
}

void NetCore::Session::RegisterDisconnect()
{
}

void NetCore::Session::ProcessDisconnect()
{
}
