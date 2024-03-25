#pragma once

NAMESPACE_OPEN(NetCore);

static LPFN_ACCEPTEX AcceptEx = nullptr;

class Listener : public IOCPObject
{
	constexpr static auto MAX_ACCEPT_COUNT = 10;
public:
	Listener(SOCKADDR_IN& addr, std::function<Session*()> session_factory, IOCPCore& core);
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
	std::function<Session* ()> _session_factory;

// TEMP
public:
	Vector<Session*> sessions;
	
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
		for (auto s : sessions)
		{
			xxdelete(s);
		}
		sessions.clear();
	}
private:
	IOCPCore& _core;
	Session* _get_new_session()
	{
		auto s = _session_factory();
		sessions.push_back(s);
		ASSERT_CRASH(_core.RegisterIOCP(s));
		return s;
	}
};

NAMESPACE_CLOSE