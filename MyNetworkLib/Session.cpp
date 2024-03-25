#include "pch.h"
#include "Session.h"

NetCore::Session::Session()
{
	// Create a async IO Socket using Overlapped model.
	_socket = ::WSASocketW(AF_INET, SOCK_STREAM,
		IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

	if (_socket == INVALID_SOCKET)
	{
		int32 errorCode = ::WSAGetLastError();
		ERR(errorCode, "Failed to create socket.");
	}


	// Check failed
	ASSERT_CRASH(_socket != INVALID_SOCKET);
}

NetCore::Session::~Session()
{
	// Close socket
	::closesocket(_socket);

	_socket = INVALID_SOCKET;
}

void NetCore::Session::SetConnected()
{
	_connected.store(true);

	OnConnected();

	RegisterRecv();
}

bool NetCore::Session::Send(const _byte* buffer)
{
	if (_connected == false)
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


void NetCore::Session::Dispatch(IOCPEvent* event, int32 numberOfBytes = 0)
{
	SHOW(EventType, (int)event->GetEventType());
	switch (event->GetEventType())
	{
	case EventType::Accept:
		break;
	case EventType::Connect:
		break;
	case EventType::Recv:
		ProcessRecv(numberOfBytes);
		break;
	case EventType::Send:
		ProcessSend(numberOfBytes);
		break;
	case EventType::Disconnect:
		ProcessDisconnect();
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


	{
		_sendLock.lock();

		// TODO : processing with queue

		_sendLock.unlock();
	}

	WSABUF sendBuffer{};
	sendBuffer.buf = reinterpret_cast<_byte*>(GetSendBuffer());
	sendBuffer.len = MAX_BUFFER_SIZE;

	DWORD numberOfBytesSent = 0;
	int32 res = WSASend(_socket, &sendBuffer, 1,
		OUT & numberOfBytesSent, 0, &_sendEvent, nullptr);
	if (res == SOCKET_ERROR)
	{
		// Check pending
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			// Error
			ERR(errorCode, "WSASend Error");
		}
	}

}

void NetCore::Session::ProcessSend(const int32 numberOfBytesSent)
{
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


	// TEMP
	WSABUF recvBuffer {};
	recvBuffer.buf = reinterpret_cast<_byte*>(GetRecvBuffer());
	recvBuffer.len = MAX_BUFFER_SIZE;

	DWORD numberOfBytesRecvd = 0; // OUT
	DWORD flags = 0;
	SHOW(connected, IsConnected());
	int32 res = ::WSARecv(_socket, &recvBuffer, 1,
		OUT & numberOfBytesRecvd, OUT & flags, &_recvEvent, nullptr);
	if (res == SOCKET_ERROR)
	{
		// Check pending
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			// Error
			ERR(errorCode, "WSARecv Error");

		}
	}

	

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
