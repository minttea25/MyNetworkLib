#pragma once

#include "PacketManager.h"
#include "SessionManager.h"

//class SessionManager;
//
//class ServerSession : public NetCore::PacketSession
//{
//public:
//	ServerSession(SessionManager* manager)
//	{
//		_id = id.fetch_add(1);
//		_manager = manager;
//	}
//	~ServerSession()
//	{
//		std::cout << "~ServerSession id=" << _id << '\n';
//	}
//public:
//	void OnConnected() override
//	{
//		std::shared_ptr<ServerSession> s = static_pointer_cast<ServerSession>(shared_from_this());
//		//GSessionManager->AddSession(s);
//		_manager->AddSession(s);
//	}
//	void OnRecvPacket(const char* buffer, const unsigned __int16 id) override
//	{
//		GPacketManager->Dispatch(id, buffer, *this);
//	}
//	virtual void OnDisconnected(const int error) override
//	{
//		std::cout << "disconnected: " << error << std::endl;
//	}
//public:
//	static atomic<int> id;
//	//bool IsConnected() const { return Session::IsConnected(); }
//	//void Flush() { PacketSession::Flush(); }
//private:
//	int _id;
//	SessionManager* _manager = nullptr;
//};

