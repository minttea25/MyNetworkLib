#include "pch.h"
#include "SessionManager.h"

class SessionManager* GSessionManager = nullptr;

atomic<int> ClientSession::id = 1;

void ClientSession::OnConnected()
{
	std::cout << "Connected" << std::endl;
	std::shared_ptr<ClientSession> s = static_pointer_cast<ClientSession>(shared_from_this());
	GSessionManager->AddSession(s);
}