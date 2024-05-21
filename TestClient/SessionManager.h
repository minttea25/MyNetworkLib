#pragma once

extern class SessionManager* GSessionManager;

class ServerSession : public NetCore::PacketSession
{
public:
	ServerSession()
	{
	}
	~ServerSession()
	{
		LOG(INFO) << "~ServerSession id=" << Session::SessionId();
	}
public:
	void OnConnected() override;
	void OnRecvPacket(const char* buffer, const unsigned __int16 id) override
	{
		GPacketManager->Dispatch(id, buffer, *this);
	}
	virtual void OnDisconnected(const int error) override
	{
		LOG(INFO) << "Disconnected id=" << Session::SessionId() << ": " << error;
	}
public:
	static atomic<int> id;
	std::string _ip;
};

class SessionManager
{
public:
	SessionManager()
	{

	}
	~SessionManager()
	{
		__NETCORE_LOG_INFO(~SessionManager);
		_sessions.clear();
	}

	void AddSession(std::shared_ptr<ServerSession> session)
	{
		lock_guard lg(mutex);
		_sessions.push_back(session);
	}

	std::shared_ptr<NetCore::Session> SessionFactory()
	{
		return NetCore::make_shared<ServerSession>();
	}

	void FlushSessions()
	{
		lock_guard lg(mutex);
		for (std::shared_ptr<ServerSession>& s : _sessions)
		{
			if (s != nullptr && s->IsConnected()) s->Flush();
		}
	}
private:
	std::mutex mutex;
	std::vector<std::shared_ptr<ServerSession>> _sessions;
};
