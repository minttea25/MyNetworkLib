#pragma once

NAMESPACE_OPEN(NetCore);

class Session : public IOCPObject
{
	constexpr static uint32 MAX_BUFFER_SIZE = 0b1000000;  // 0x10000;

	enum DisconnectError
	{
		NONE,
		SENT_0,

		RECV_0,
		RECV_BUFFER_WRTIE_OVERFLOW,
		PROCESS_LEN_0,
		RECV_BUFFER_READ_OVERFLOW,
	};
public:
	Session();
	~Session();

	bool IsConnected() const { return _connected; }

public:
	void SetConnected();

	bool Send(const _byte* buffer);
	bool Disconnect();
	SOCKET GetSocket() const { return _socket; }

	_byte* GetRecvBuffer() { return _recvBuffer; }
	_byte* GetSendBuffer() { return _sendBuffer; }
private:
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
	SOCKET _socket = INVALID_SOCKET;
	uint32 _sessionId = 0;
	Mutex _sendLock;
	Atomic<bool> _connected = false;

	SendEvent _sendEvent{ }; // overlapped event used as sending
	RecvEvent _recvEvent{ }; // overlapped event used as receiving
	DisconnectEvent _disconnectEvent{ }; // overlapped event used as disconnecting

	_byte _recvBuffer[MAX_BUFFER_SIZE] = { 0, };
	_byte _sendBuffer[MAX_BUFFER_SIZE] = { 0, };

	friend class Connector;
	friend class Listener;


public: // TEMP
	//Listener* listener; // temp it can be nullptr later.
};

NAMESPACE_CLOSE;

