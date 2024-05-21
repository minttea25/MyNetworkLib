#include "pch.h"

#include "ServerSession.h"
#include "SessionManager.h"
#include "PacketManager.h"

class SessionManager* GSessionManager = nullptr;

atomic<int> ServerSession::id = 1;

void ServerSession::OnConnected()
{
	wchar_t b[30]{};
	UINT16 port = 0;

	if (!Session::GetPeerName(&port, b, sizeof(b)))
	{
		// error

	}
	_ip = NetCore::Utils::ToString(b);
	LOG(INFO) << "OnConnected [id=" << Session::SessionId() << ']' << "IP: " << _ip;

	std::shared_ptr<ServerSession> s = static_pointer_cast<ServerSession>(shared_from_this());
	GSessionManager->AddSession(s);
}
