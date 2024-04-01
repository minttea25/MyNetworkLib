#pragma once

NAMESPACE_OPEN(NetCore);

/// <summary>
/// Listener is a class to wait external connection. (bind / listen)
/// </summary>
class Listener : public IOCPObject
{
	static constexpr auto MAX_ACCEPT_COUNT = 1;
public:
	Listener(SOCKADDR_IN& addr, std::function<SessionSPtr()> session_factory, IOCPCoreSPtr core);
	~Listener();

	/// <summary>
	/// Start to listen.
	/// </summary>
	/// <param name="backlog">backlog</param>
	/// <returns>true if trying to bind and listen successful, false otherwise</returns>
	bool StartListen(const int32 backlog);
public:
	size_t GetConnectedSessionCount() const { return _accepEvents.size(); }
private:
	// Inherited via IOCPObject
	void Process(IOCPEvent* overlappedEvent, DWORD numberOfBytesTransferred) override;
	HANDLE GetHandle() override;

	void RegisterAccept(AcceptEvent* acceptEvent);
	void ProcessAccept(AcceptEvent* acceptEvent);
private:
	SOCKET _listenSocket = INVALID_SOCKET;
	SOCKADDR_IN _addr;
	Vector<AcceptEvent*> _accepEvents; // Managed only by Listener
	std::function<SessionSPtr()> _session_factory;
	IOCPCoreSPtr _core;

// TEMP
public:
	Set<SessionSPtr> sessions;
	
	void BroadCast(const char* msg)
	{
		for (auto s : sessions)
		{
			s->Send(msg);
		}
	}

	void ReleaseAllSessions()
	{
		sessions.clear();
	}
	
	friend class Session;
};

NAMESPACE_CLOSE