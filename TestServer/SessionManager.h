#pragma once

extern std::shared_ptr<class SessionManager> GSessionManager;


class ClientSession : public NetCore::PacketSession
{
public:
	ClientSession()
	{
	}
	~ClientSession()
	{
		LOG(INFO) << "~ClientSession id=" << Session::SessionId();
	}

	void OnConnected() override;

	void OnRecvPacket(const char* buffer, const unsigned __int16 id) override;

	void OnDisconnected(const int error) override
	{
		LOG(INFO) << "Disconnected id=" << Session::SessionId() << " by Error: " << error;
	}
public:
	static atomic<int> id;
	std::string _ip;
};

class SessionManager : public NetCore::JobSerializer
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

	void AddSession(std::shared_ptr<ClientSession> session)
	{
		lock_guard lg(mutex);
		_sessions.push_back(session);
	}

	std::shared_ptr<NetCore::Session> SessionFactory()
	{
		return NetCore::make_shared<ClientSession>();
	}

	void FlushSessions()
	{
		std::cerr << "FlushSessions on tid=" << NetCore::TLS_Id << '\n';
		lock_guard lg(mutex);
		for (std::shared_ptr<ClientSession> s : _sessions)
		{
			if (s != nullptr && s->IsConnected()) s->Flush();
		}
	}
private:
	std::mutex mutex;
	std::vector<std::shared_ptr<ClientSession>> _sessions;
};
