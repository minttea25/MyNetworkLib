#include "pch.h"
#include "SessionManager.h"

class SessionManager* GSessionManager = nullptr;

atomic<int> ClientSession::id = 1;

void ClientSession::OnConnected()
{
	wchar_t b[30]{};
	UINT16 port = 0;

	if (!Session::GetPeerName(&port, b, sizeof(b)))
	{
		// error
	}

	_ip = NetCore::Utils::ToString(b);
	LOG(INFO) << "OnConnected [id=" << Session::SessionId() << ']' << "IP: " << _ip;


	std::shared_ptr<ClientSession> s = static_pointer_cast<ClientSession>(shared_from_this());
	GSessionManager->AddSession(s);
}