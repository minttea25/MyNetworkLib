#pragma once

NAMESPACE_OPEN(NetCore);

/// <summary>
/// Listener is a class to wait external connection. (bind / listen)
/// </summary>
class Listener : public IOCPObject
{
	static constexpr auto MAX_ACCEPT_COUNT = 100;
public:
	Listener(ServiceSPtr serverService);
	~Listener();

	/// <summary>
	/// Start to listen.
	/// </summary>
	/// <param name="backlog">backlog</param>
	/// <returns>true if trying to bind and listen successful, false otherwise</returns>
	bool StartListen(const int32 backlog);
private:
	// Inherited via IOCPObject
	void Dispatch(IOCPEvent* overlappedEvent, DWORD numberOfBytesTransferred) override;
	HANDLE GetHandle() override;

	void RegisterAccept(AcceptEvent* acceptEvent);
	void ProcessAccept(AcceptEvent* acceptEvent);
private:
	ServiceSPtr _serverService = nullptr;
	SOCKET _listenSocket = INVALID_SOCKET;
	Vector<AcceptEvent*> _acceptEvents; // Managed only by Listener

	friend class Session;
};

NAMESPACE_CLOSE