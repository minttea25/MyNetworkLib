#include "pch.h"

#include "ServerSession.h"
#include "SessionManager.h"
#include "PacketManager.h"


class SessionManager* GSessionManager = nullptr;

atomic<int> ServerSession::id = 1;

void ServerSession::OnConnected()
{
	std::cout << "Connected" << std::endl;
	std::shared_ptr<ServerSession> s = static_pointer_cast<ServerSession>(shared_from_this());
	GSessionManager->AddSession(s);
}
