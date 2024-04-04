#pragma once

NAMESPACE_OPEN(NetCore);

enum ServiceType : uint8
{
	None,
	Client,
	Server,
};

using SessionFactory = std::function<SessionSPtr()>;

ABSTRACT class Service : public std::enable_shared_from_this<Service>
{
public:
	Service(ServiceType serviceType, IOCPCoreSPtr iocpCore, SOCKADDR_IN sockAddr, SessionFactory sessionFactory, const uint32 sessionCount);
	virtual ~Service() noexcept;

	Service(const Service&) = delete;
	Service(Service&&) noexcept = delete;
	Service& operator=(const Service&) = delete;
	Service& operator=(Service&&) noexcept = delete;

public:
	virtual bool Start() PURE_VIRTUAL;
	virtual bool Stop() PURE_VIRTUAL;
	virtual void Broadcast(const char* msg) PURE_VIRTUAL;

	
	inline IOCPCoreSPtr GetIOCPCore() const { return _iocpCore; }
	inline ServiceType GetServiceType() const { return _serviceType; }
	inline bool IsStarted() const { return _onGoing; }
	inline uint32 GetMaxSessionCount() const { return MAX_SESSION_COUNT; }
	inline SOCKADDR_IN GetAddr() const { return _addr; }

	virtual uint32 GetCurrentSessionCount() const PURE_VIRTUAL;
	virtual SessionSPtr AddNewSession() PURE_VIRTUAL;
protected:
	virtual bool ReleaseSession(SessionSPtr session_s_ptr) PURE_VIRTUAL;
	virtual void ReleaseAllSessions() PURE_VIRTUAL;
private:
	const uint32 MAX_SESSION_COUNT;

	ServiceType _serviceType = ServiceType::None;
	IOCPCoreSPtr _iocpCore = nullptr;
	sockaddr_in _addr;
	SessionFactory _sessionFactory;

	bool _onGoing = false;

	friend class Listener;
	friend class Session;
	friend class Connector;
	friend class ClientService;
	friend class ServerService;
};

/// <summary>
/// This class is for connection to Server with single session.
/// </summary>
class ClientService : public Service
{
public:
	ClientService(IOCPCoreSPtr iocpCore, SOCKADDR_IN addr, SessionFactory sessionFactory);
	virtual ~ClientService() noexcept;
public:
	static constexpr inline auto MAX_SESSION_COUNT = 1;

	// Inherited via Service
	
	/// <summary>
	/// Start service. (= Start to connect to address)
	/// </summary>
	/// <returns>true if successful, false otherwise.</returns>
	bool Start() override;
	/// <summary>
	/// Stop service and release a session if it is connected.
	/// </summary>
	/// <returns>true if successful, false otherwise.</returns>
	bool Stop() override;
	/// <summary>
	/// Send a message to connected address.
	/// </summary>
	/// <param name="msg">Message to send.</param>
	void Broadcast(const char* msg) override;
	uint32 GetCurrentSessionCount() const override;
	/// <summary>
	/// Create a new session with session factory and register its handle in iocp core.
	/// </summary>
	/// <returns>The created new session if successful, false otherwise.</returns>
	SessionSPtr AddNewSession() override;
protected:
	bool ReleaseSession(SessionSPtr session_s_ptr) override;
	void ReleaseAllSessions() override;
private:
	SessionSPtr _session = nullptr;
	ConnectorSPtr _connector = nullptr;

	friend class Connector;
};

class ServerService : public Service
{
public:
	ServerService(IOCPCoreSPtr iocpCore, SOCKADDR_IN addr, SessionFactory sessionFactory, const uint32 sessionCount, const uint32 backlog);
	virtual ~ServerService() noexcept;
public:
	const uint32 BACK_LOG;

	// Inherited via Service

	/// <summary>
	/// Start service. (= Start to listen)
	/// </summary>
	/// <returns>true if successful, false otherwise.</returns>
	bool Start() override;
	/// <summary>
	/// Stop service and release all sessions.
	/// </summary>
	/// <returns>true if successful, false otherwise.</returns>
	bool Stop() override;
	/// <summary>
	/// Send msg to all connected sessions.
	/// </summary>
	/// <param name="msg">Message to send.</param>
	void Broadcast(const char* msg) override;
	uint32 GetCurrentSessionCount() const override { return _sessions.size(); }
	/// <summary>
	/// Create a new session with session factory and register its handle in iocp core.
	/// </summary>
	/// <returns>New created session if successful, nullptr otherwise.</returns>
	SessionSPtr AddNewSession() override;

protected:
	bool ReleaseSession(SessionSPtr session_s_ptr) override;
	void ReleaseAllSessions() override;
private:
	Set<SessionSPtr> _sessions;
	ListenerSPtr _listener = nullptr;

	

	friend class Listener;
};

NAMESPACE_CLOSE;
