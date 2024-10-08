#include "pch.h"
#include "Service.h"

NetCore::Service::Service(ServiceType serviceType, IOCPCoreSPtr iocpCore, SOCKADDR_IN sockAddr, SessionFactory sessionFactory, const uint32 sessionCount)
	: _serviceType(serviceType), _iocpCore(iocpCore), _addr(sockAddr),
	_sessionFactory(sessionFactory), MAX_SESSION_COUNT(sessionCount)
{

}

NetCore::Service::~Service()
{
	DESTRUCTOR(Service);

	_iocpCore = nullptr;
	_sessionFactory = nullptr;
	_serviceType = ServiceType::None;
}

NetCore::ClientService::ClientService(IOCPCoreSPtr iocpCore, SOCKADDR_IN addr, SessionFactory sessionFactory)
	: Service(ServiceType::Client, iocpCore, addr, sessionFactory, MAX_SESSION_COUNT)
{
}

NetCore::ClientService::~ClientService() noexcept
{
	MESSAGE(~ClientService);
}


bool NetCore::ClientService::Start()
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

	// Create connector as shared_ptr
	_connector = NetCore::make_shared<Connector>(shared_from_this());

	// Try to connect to end point
	bool suc = _connector->Connect();
	if (Errors::NONE != ErrorHandler::CheckError(suc, APP_CLIENTSERVICE_CONNECT_FAILED))
	{
		return false;
	}

	Service::_onGoing = true;

	return true;
}

bool NetCore::ClientService::Stop()
{
	_connector = nullptr;
	
	ReleaseAllSessions();

	Service::_onGoing = false;

	return true;
}

void NetCore::ClientService::Send_(const uint16 id, _ubyte* ptr, const uint16 size)
{
	if (_session != nullptr && _session->IsConnected())
	{
		_session->Send(id, ptr, size);
	}
}

size_t NetCore::ClientService::GetCurrentSessionCount() const
{
	if (_session == nullptr || _session->IsConnected() == false) return 0;
	else return 1;
}

NetCore::SessionSPtr NetCore::ClientService::AddNewSession()
{
	_session = Service::_sessionFactory();
	if (Service::_iocpCore->RegisterHandle(_session) == false)
	{
		return nullptr;
	}

	return _session;
}

bool NetCore::ClientService::ReleaseSession(SessionSPtr session_s_ptr)
{
	if (_session == nullptr) return false;
	else
	{
		_session = nullptr; return true;
	}
}

void NetCore::ClientService::ReleaseAllSessions()
{
	_session = nullptr;
}





NetCore::ServerService::ServerService(IOCPCoreSPtr iocpCore, SOCKADDR_IN addr, SessionFactory sessionFactory, const uint32 backlog, const uint32 sessionCount)
	: Service(ServiceType::Server, iocpCore, addr, sessionFactory, sessionCount),
	BACK_LOG(backlog)
{
}

NetCore::ServerService::~ServerService() noexcept
{
	MESSAGE(~ServerService);
}

bool NetCore::ServerService::Start()
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

	_listener = NetCore::make_shared<Listener>(shared_from_this());

	// Try to listen
	bool suc = _listener->StartListen(BACK_LOG);
	if (Errors::NONE != ErrorHandler::CheckError(suc, APP_SERVERSERVICE_LISTEN_FAILED))
	{
		return false;
	}

	Service::_onGoing = true;

	return true;
}

bool NetCore::ServerService::Stop()
{
	Service::_onGoing = false;
	_listener = nullptr;
	ReleaseAllSessions();
	return true;
}

void NetCore::ServerService::Broadcast(const uint16 id, _ubyte* ptr, const uint16 size)
{
	for (const auto& session : _sessions)
	{
		if (session->IsConnected())
		{
			session->Send(id, ptr, size);
		}
	}
}

NetCore::SessionSPtr NetCore::ServerService::AddNewSession()
{
	auto session = Service::_sessionFactory();

	if (Service::_iocpCore->RegisterHandle(session) == false)
	{
		return nullptr;
	}
	_sessions.insert(session);

	return session;
}

bool NetCore::ServerService::ReleaseSession(SessionSPtr session_s_ptr)
{
	auto erased_count = _sessions.erase(session_s_ptr);
	return erased_count == 1;
}

void NetCore::ServerService::ReleaseAllSessions()
{
	// Disconnect sessions
	for (auto& s : _sessions)
	{
		if (s->IsConnected() == true) s->_disconnect(Session::DisconnectError::SERVER_SERVICE_STOPPED);
	}

	_sessions.clear();
}
