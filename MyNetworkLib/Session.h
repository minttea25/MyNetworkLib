#pragma once

NAMESPACE_OPEN(NetCore);

class Session : public IOCPObject
{
	static constexpr uint32 RECV_BUFFER_SIZE = 65536; // 64 kb

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
	PURE_VIRTUAL virtual void SendRaw(const _byte* buffer) = 0;
	PURE_VIRTUAL virtual void Send(const uint16 id, _ubyte* ptr, const uint16 size) = 0;

	void _send(Vector<WSABUF>& buffers);
	bool Disconnect();
	SOCKET GetSocket() const { return _socket; }
private:
	void _set_connected(const ServiceSPtr service, const Socket connectedSocket = INVALID_SOCKET);
	void _set_socket(Socket connectedSocket);
	void _disconnect(const uint16 errorCode = DisconnectError::NONE);

	// Inherited via IOCPObject
	virtual void Dispatch(IOCPEvent* overlappedEvent, DWORD numberOfBytesTransferred) override sealed;
	HANDLE GetHandle() override;
private:
	void _register_send();
	void _process_send(const int32 numberOfBytesSent);
	void _register_recv();
	void _process_recv(const uint32 numberOfBytesRecvd);
	bool _register_disconnect();
	void _process_disconnect();
protected: // virtuals
	virtual void OnConnected() { }
	virtual void OnSend(const int32 len) { }
	virtual uint32 OnRecv(const _byte* buffer, const uint32 len) { return len; }
	virtual void OnDisconnected(const int32 error = DisconnectError::NONE) { }
private:
	USE_LOCK(send);
	ServiceSPtr _service = nullptr;
	SOCKET _socket = INVALID_SOCKET;
	uint32 _sessionId = 0;
	Atomic<bool> _connected = false;

	/// <summary>
	/// overlapped event used as sending
	/// </summary>
	SendEvent _sendEvent{ };
	/// <summary>
	/// overlapped event used as receiving
	/// </summary>
	RecvEvent _recvEvent{ };
	/// <summary>
	/// overlapped event used as disconnecting
	/// </summary>
	DisconnectEvent _disconnectEvent{ };

	RecvBuffer _recvBuffer;
	Vector<WSABUF> _sendQueue;
	Atomic<bool> _sending = false;

	friend class Connector;
	friend class Listener;
	friend class ServerService;
};

NAMESPACE_CLOSE;

