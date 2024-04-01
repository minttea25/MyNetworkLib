#pragma once

NAMESPACE_OPEN(NetCore);

/// <summary>
/// Listener is a class to wait external connection. (bind / listen)
/// </summary>
class Listener : public IOCPObject
{
	static constexpr auto MAX_ACCEPT_COUNT = 10;
public:
	Listener(SOCKADDR_IN& addr, std::function<SessionSPtr()> session_factory, IOCPCore& core);
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
	Vector<AcceptEvent*> _accepEvents;
	std::function<SessionSPtr()> _session_factory;

// TEMP
public:
	Vector<SessionSPtr> sessions;
	
	void BroadCast(const char* msg)
	{
		// IT'S TEMP
		// It must be used with lock
		for (auto s : sessions)
		{
			if(s->IsConnected() == true) 
				s->Send(msg);
		}
	}

	void ReleaseAllSessions()
	{
		sessions.clear();
	}
private:
	IOCPCore& _core;
};

NAMESPACE_CLOSE