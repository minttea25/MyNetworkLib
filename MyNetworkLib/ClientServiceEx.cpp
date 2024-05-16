#include "pch.h"
#include "ClientServiceEx.h"

NetCore::ClientServiceEx::ClientServiceEx(IOCPCoreSPtr iocpCore, SOCKADDR_IN addr, SessionFactory sessionFactory, const uint32 sessionCount)
	: Service(ServiceType::Client, iocpCore, addr, sessionFactory, sessionCount)
{
}

NetCore::ClientServiceEx::~ClientServiceEx()
{
	
}

bool NetCore::ClientServiceEx::Start()
{
	// Check conditions -----------------------------------
	if (Errors::NONE != ErrorHandler::CheckError(Service::_onGoing == false, Errors::APP_SERVICE_IS_ALREADY_STARTED))
	{
		return false;
	}

	if (Errors::NONE != ErrorHandler::CheckError(Service::_iocpCore != nullptr, Errors::APP_SERVICE_IOCPCORE_NULLPTR))
	{
		return false;
	}

	if (Errors::NONE != ErrorHandler::CheckError(Service::_sessionFactory != nullptr, Errors::APP_SERVICE_SESSIONFACTORY_NULLPTR))
	{
		return false;
	}

	if (Errors::NONE != ErrorHandler::CheckError(AddrUtils::IsValid(Service::_addr), Errors::APP_SERVICE_SOCKADDR_INVALID))
	{
		return false;
	}

	// condition check end ------------------------------------

	for (uint32 i = 0; i < Service::MAX_SESSION_COUNT; ++i)
	{
		ConnectorSPtr conn = ObjectPool<Connector>::make_shared(shared_from_this());
		_connectors.push_back(conn);
		bool suc = conn->Connect();
		if (Errors::NONE != ErrorHandler::CheckError(suc, APP_CLIENTSERVICE_CONNECT_FAILED))
		{
			return false;
		}
	}

	Service::_onGoing = true;

	return true;
}

bool NetCore::ClientServiceEx::Stop()
{
	for (auto& conn : _connectors) conn = nullptr;
	_connectors.clear();

	ReleaseAllSessions();

	Service::_onGoing = false;

	return true;
}

void NetCore::ClientServiceEx::DisconnectAll()
{
	for (auto& s : _sessions) 
		s->Disconnect();
}

void NetCore::ClientServiceEx::ReleaseAllSessions()
{
	for (auto& s : _sessions) s = nullptr;
	_sessions.clear();
}

NetCore::SessionSPtr NetCore::ClientServiceEx::AddNewSession()
{
	SessionSPtr session = Service::_sessionFactory();
	_sessions.push_back(session);

	if (Service::_iocpCore->RegisterHandle(session) == false)
	{
		return nullptr;
	}
	return session;
}

void NetCore::ClientServiceEx::Send(const uint16 id, _ubyte* ptr, const uint16 size)
{
	for (auto& session : _sessions)
	{
		if (session != nullptr && session->IsConnected())
		{
			session->Send(id, ptr, size);
		}
	}
}

size_t NetCore::ClientServiceEx::GetCurrentSessionCount() const
{
	return Service::MAX_SESSION_COUNT;
}

bool NetCore::ClientServiceEx::ReleaseSession(SessionSPtr session_s_ptr)
{
	auto session = find(_sessions.begin(), _sessions.end(), session_s_ptr);
	if (session == _sessions.end()) return false;
	else
	{
		*session = nullptr;
		return true;
	}
}
