#pragma once

NAMESPACE_OPEN(NetCore);

/// <summary>
/// ClinetService extension class for mulitple connections.
/// </summary>
class ClientServiceEx : public Service
{
public:
	ClientServiceEx(IOCPCoreSPtr iocpCore, SOCKADDR_IN addr, SessionFactory sessionFactory, const uint32 sessionCount);
	~ClientServiceEx() noexcept;

	bool Start() override;
	bool Stop() override;
	
	void DisconnectAll();
	void ReleaseAllSessions() override;
	SessionSPtr AddNewSession() override;

	void Send(const uint16 id, _ubyte* ptr, const uint16 size);
private:
	Vector<SessionSPtr> _sessions;
	Vector<ConnectorSPtr> _connectors;

	// Inherited via Service
	size_t GetCurrentSessionCount() const override;
	bool ReleaseSession(SessionSPtr session_s_ptr) override;
};

NAMESPACE_CLOSE;

