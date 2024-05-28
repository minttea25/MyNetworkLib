#include "pch.h"
#include "SessionManager.h"

std::shared_ptr<class SessionManager> GSessionManager = nullptr;

atomic<int> ClientSession::id = 1;

void ClientSession::OnConnected()
{
	wchar_t b[INET_ADDRSTRLEN]{};
	UINT16 port = 0;

	if (Session::GetPeerName(&port, b, INET_ADDRSTRLEN) == false)
	{
		// error
		std::cerr << "Failed to get peer name. WSALastError: " << ::WSAGetLastError() << '\n';
	}

	_ip = NetCore::Utils::ToString(b);
	LOG(INFO) << "OnConnected [id=" << Session::SessionId() << ']' << "IP: " << _ip;


	std::shared_ptr<ClientSession> s = static_pointer_cast<ClientSession>(shared_from_this());
	GSessionManager->AddSession(s);
}

void ClientSession::OnRecvPacket(const char* buffer, const unsigned __int16 id)
{
	GPacketManager->Dispatch(id, buffer, *this);
}
