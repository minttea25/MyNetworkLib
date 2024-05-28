#include "pch.h"

#include "SessionManager.h"
#include "PacketManager.h"

class SessionManager* GSessionManager = nullptr;

atomic<int> ServerSession::id = 1;

void ServerSession::OnConnected()
{
	if (NetCore::SocketUtils::SetUpdateConnectContext(Session::GetSocket()) == false)
	{
		// error
		std::cerr << "Failed to udpate socket: SO_UPDATE_CONNECT_CONTEXT. WSALastError: " << ::WSAGetLastError() << '\n';
	}

	wchar_t b[INET_ADDRSTRLEN]{};
	UINT16 port = 0;

	if (Session::GetPeerName(&port, b, INET_ADDRSTRLEN) == false)
	{
		// error
		std::cerr << "Failed to get peer name." << '\n';
	}
	_ip = NetCore::Utils::ToString(b);
	LOG(INFO) << "OnConnected [id=" << Session::SessionId() << ']' << "IP: " << _ip;
	std::cerr << "OnConnected [id=" << Session::SessionId() << ']' << "IP: " << _ip << '\n';

	std::shared_ptr<ServerSession> s = static_pointer_cast<ServerSession>(shared_from_this());
	GSessionManager->AddSession(s);
}
