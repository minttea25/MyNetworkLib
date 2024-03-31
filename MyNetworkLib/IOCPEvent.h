#pragma once

NAMESPACE_OPEN(NetCore);

class IOCPObject;
class Session;

enum struct EventType : uint8
{
	Connect,
	Disconnect,
	Accept,
	Send,
	Recv,
	// ...
};

struct IOCPEvent : public _OVERLAPPED
{
public:
	IOCPEvent(EventType eventType) : _eventType(eventType)
	{
		_init();
		//Init();
	}
	inline void Clear() { _init(); }
public: // virtual
	//virtual void Init() {}
public: // property
	EventType GetEventType() const
	{
		return _eventType;
	}
	IOCPObjectSPtr GetIOCPObjectRef() { return _iocpObject; }
	LPOVERLAPPED overlapped()
	{
#ifdef DEBUG
		auto ret = static_cast<LPOVERLAPPED>(this);
		return ret;
#endif // DEBUG
		return static_cast<LPOVERLAPPED>(this);
	}
	LPOVERLAPPED* p_overlapped()
	{
#ifdef DEBUG
		auto ret = reinterpret_cast<LPOVERLAPPED*>(this);
		return ret;
#endif // DEBUG
		return reinterpret_cast<LPOVERLAPPED*>(this);
	}

	void SetIOCPObjectRef(IOCPObjectSPtr obj) noexcept
	{
		_iocpObject = obj;
	}
private:
	inline void _init()
	{
		// Initialize OVERLAPPED
		OVERLAPPED::Internal = 0;
		OVERLAPPED::InternalHigh = 0;
		OVERLAPPED::Offset = 0;
		OVERLAPPED::OffsetHigh = 0;
		OVERLAPPED::hEvent = NULL;
		// Note: In IOCPEvent, the memory location of OVERLAPPED is at the very front.
		//::ZeroMemory(this, sizeof(WSAOVERLAPPED));
	}
private:
	EventType _eventType;
	IOCPObjectSPtr _iocpObject = nullptr;
};

struct ConnectEvent : public IOCPEvent
{
public:
	ConnectEvent() : IOCPEvent(EventType::Connect) {}
	// no overrides
};

struct DisconnectEvent : public IOCPEvent
{
public:
	DisconnectEvent() : IOCPEvent(EventType::Disconnect) {}
};

struct AcceptEvent : public IOCPEvent
{
public:
	AcceptEvent() : IOCPEvent(EventType::Accept) {}

	void SetSessionRef(SessionSPtr session) noexcept
	{
		_session = session;
	}
	SessionSPtr GetSessionRef() const
	{ 
		return _session; 
	}
private:
	SessionSPtr _session = nullptr;
};

struct SendEvent : public IOCPEvent
{
public:
	SendEvent() : IOCPEvent(EventType::Send) {}
};

struct RecvEvent : public IOCPEvent
{
public:
	RecvEvent() : IOCPEvent(EventType::Recv) {}
};

NAMESPACE_CLOSE;

