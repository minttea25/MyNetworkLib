#pragma once

extern class SessionManager* GSessionManager;

class ServerSession : public NetCore::PacketSession
{
public:
	ServerSession()
	{
		_id = id.fetch_add(1);
	}
	~ServerSession()
	{
		LOG(INFO) << "~ServerSession id=" << _id;
		//std::cout << "~ServerSession id=" << _id << '\n';
	}
public:
	void OnConnected() override;
	void OnRecvPacket(const char* buffer, const unsigned __int16 id) override
	{
		GPacketManager->Dispatch(id, buffer, *this);
	}
	virtual void OnDisconnected(const int error) override
	{
		LOG(INFO) << "Disconnected id=" << _id << ": " << error;
		//std::cout << "disconnected: " << error << std::endl;
	}
public:
	static atomic<int> id;
	//bool IsConnected() const { return Session::IsConnected(); }
	//void Flush() { PacketSession::Flush(); }
private:
	int _id;
};

class SessionManager
{
public:
	SessionManager()
	{

	}
	~SessionManager()
	{
		_sessions.clear();
		std::cout << "~SessionManager" << std::endl;
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
		for (std::shared_ptr<ServerSession> s : _sessions)
		{
			if (s != nullptr && s->IsConnected()) s->Flush();
		}
	}
private:
	std::mutex mutex;
	std::vector<std::shared_ptr<ServerSession>> _sessions;
};
