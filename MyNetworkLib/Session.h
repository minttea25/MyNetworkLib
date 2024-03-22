#pragma once


NAMESPACE_OPEN(NetCore);


class Session : public IOCPObject
{
	constexpr static uint32 MAX_BUFFER_SIZE = 0x100;  // 0x10000;

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
	virtual ~Session();

public:
	void SetConnected();

	bool Send(const _byte* buffer);
	void Disconnect(uint16 errorCode);
	SOCKET GetSocket() const { return _socket; }

	_byte* GetRecvBuffer() { return _recvBuffer; }
	_byte* GetSendBuffer() { return _sendBuffer; }
private:
	// Inherited via IOCPObject
	virtual void Dispatch(struct IOCPEvent* event, int32 numberOfBytes) override sealed;
	HANDLE GetHandle() override;
private:
	void RegisterSend();
	void ProcessSend(const int32 numberOfBytesSent);
	void RegisterRecv();
	void ProcessRecv(const uint32 numberOfBytesRecvd);
	void RegisterDisconnect();
	void ProcessDisconnect();
protected: // virtuals
	virtual void OnConnected() {}
	virtual void OnSend(const int32 len) {}
	virtual uint32 OnRecv(const _byte* buffer, const uint32 len) { return 0; }
	virtual void OnDisconnected(const int32 error = 0) {}
private:
	SOCKET _socket = INVALID_SOCKET;

private:
	Mutex _sendLock;
	Atomic<bool> _connected = false;

	SendEvent _sendEvent{ };
	RecvEvent _recvEvent{ };

	_byte _recvBuffer[MAX_BUFFER_SIZE] = { 0, };
	_byte _sendBuffer[MAX_BUFFER_SIZE] = { 0, };

	friend class Connector;
	friend class Listener;
};

NAMESPACE_CLOSE;

