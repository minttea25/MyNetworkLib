#pragma once

NAMESPACE_OPEN(NetCore);

static LPFN_ACCEPTEX AcceptEx = nullptr;

class Listener : public IOCPObject
{
	constexpr static auto MAX_ACCEPT_COUNT = 10;
public:
	Listener(SOCKADDR_IN& addr);
	~Listener();

	bool StartListen();
public:
	size_t GetConnectedSessionCount() const { return _accepEvents.size(); }
private:
	// Inherited via IOCPObject
	void Dispatch(IOCPEvent* event, int32 numberOfBytes) override;
	HANDLE GetHandle() override;

	void _initWSockFunctions();
	void RegisterAccept(AcceptEvent* acceptEvent);
	void ProcessAccept(AcceptEvent* acceptEvent);
private:
	SOCKET _listenSocket = INVALID_SOCKET;
	SOCKADDR_IN _addr;
	Vector<AcceptEvent*> _accepEvents;

// TEMP
public:
	Vector<Session*> sessions;
	Session* GetNewSession() noexcept
	{
		Session* s = xxnew<Session>();
		sessions.push_back(s);
		return s;
	}

	void ReleaseAllSessions()
	{
		for (auto s : sessions)
		{
			xxdelete(s);
		}
		sessions.clear();
	}

};

NAMESPACE_CLOSE