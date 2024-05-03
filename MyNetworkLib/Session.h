#pragma once

NAMESPACE_OPEN(NetCore);

class Session : public IOCPObject
{
	// TEMP size
	constexpr static uint32 MAX_BUFFER_SIZE = 0b1000000;  // 0x10000;

	enum DisconnectError
	{
		NONE,

		SERVER_SERVICE_STOPPED,
		CLIENT_SERVICE_STOPPED,

		SENT_0,

		RECV_0,
		RECV_BUFFER_WRTIE_OVERFLOW,
		PROCESS_LEN_0,
		RECV_BUFFER_READ_OVERFLOW,
	};
public:
	Session();
	virtual ~Session();

	bool IsConnected() const { return _connected; }

public:
	virtual void Send(const _byte* buffer) PURE_VIRTUAL;
	virtual void Send_(const ushort id, _ubyte* ptr, const ushort size) PURE_VIRTUAL;

	void _send(Vector<WSABUF>& buffers);
	bool Disconnect();
	SOCKET GetSocket() const { return _socket; }

	_byte* GetRecvBuffer() { return _recvBuffer; }
private:
	void SetConnected(const ServiceSPtr service, const Socket connectedSocket = INVALID_SOCKET);
	void _set_socket(Socket connectedSocket);
	void _disconnect(uint16 errorCode = DisconnectError::NONE);

	// Inherited via IOCPObject
	virtual void Process(IOCPEvent* overlappedEvent, DWORD numberOfBytesTransferred) override sealed;
	HANDLE GetHandle() override;
private:
	void RegisterSend();
	void ProcessSend(const int32 numberOfBytesSent);
	void RegisterRecv();
	void ProcessRecv(const uint32 numberOfBytesRecvd);
	bool RegisterDisconnect();
	void ProcessDisconnect();
protected: // virtuals
	
	virtual void OnConnected() { std::cout << "OnConnected at Session." << std::endl; }
	virtual void OnSend(const int32 len) { std::cout << "Sent: " << len << " bytes" << std::endl; }
	virtual uint32 OnRecv(const _byte* buffer, const uint32 len) { std::cout << "Received: " << len << " bytes" << std::endl; return len; }
	virtual void OnDisconnected(const int32 error = DisconnectError::NONE) { std::cout << "OnDisconnected: " << error << std::endl; }
private:
	USE_LOCK(send);
	ServiceSPtr _service = nullptr;
	SOCKET _socket = INVALID_SOCKET;
	uint32 _sessionId = 0;
	Atomic<bool> _connected = false;

	SendEvent _sendEvent{ }; // overlapped event used as sending
	RecvEvent _recvEvent{ }; // overlapped event used as receiving
	DisconnectEvent _disconnectEvent{ }; // overlapped event used as disconnecting

	_byte _recvBuffer[MAX_BUFFER_SIZE] = { 0, };
	//_byte _sendBuffer[MAX_BUFFER_SIZE] = { 0, };
	Vector<WSABUF> _sendQueue;
	Atomic<bool> _sending = false;

	friend class Connector;
	friend class Listener;
	friend class ServerService;
};

NAMESPACE_CLOSE;

