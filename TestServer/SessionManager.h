#pragma once

extern class SessionManager* GSessionManager;


class ClientSession : public NetCore::PacketSession
{
public:
	ClientSession()
	{
		_id = id.fetch_add(1);
	}
	~ClientSession()
	{
		LOG(INFO) << "~ClientSession id=" << _id;
		//std::cout << "~ClientSession id=" << _id << '\n';
	}

	void OnConnected() override;

	void OnRecvPacket(const char* buffer, const unsigned __int16 id) override
	{
		GPacketManager->Dispatch(id, buffer, *this);
	}

	void OnDisconnected(const int error) override
	{
		LOG(INFO) << "Disconnected id=" << _id << ": " << error;
		//std::cout << "disconnected: " << error << std::endl;
	}
public:
	static atomic<int> id;
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
