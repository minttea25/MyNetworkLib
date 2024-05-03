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

/// <summary>
/// Note: DO NOT USE VIRTUAL IN IOCPEVENT!
/// </summary>
ABSTRACT struct IOCPEvent : public _OVERLAPPED
{
public:
	IOCPEvent(EventType eventType) : _eventType(eventType)
	{
		_init();
	}
	~IOCPEvent()
	{
		ReleaseIOCPObjectSPtr();
	}
	inline void Clear() { _init(); }
	inline void ReleaseIOCPObjectSPtr() {}//_iocpObject = nullptr; }
public:
	EventType GetEventType() const
	{
		return _eventType;
	}
	IOCPObjectWPtr GetIOCPObjectWPtr() { return _iocpObject; }
	LPOVERLAPPED overlapped()
	{
		return static_cast<LPOVERLAPPED>(this);
	}
	LPOVERLAPPED* p_overlapped()
	{
		return reinterpret_cast<LPOVERLAPPED*>(this);
	}

	void SetIOCPObjectSPtr(IOCPObjectSPtr obj) noexcept
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
	IOCPObjectWPtr _iocpObject;
};

struct ConnectEvent : public IOCPEvent
{
public:
	ConnectEvent() : IOCPEvent(EventType::Connect) {}

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
	SendEvent() : IOCPEvent(EventType::Send)
	{
		Init();
	}
	void Init()
	{
		_segments.clear();
	}
private:
	//Vector<std::shared_ptr<SendBufferSegment>> _segments;
	Vector<WSABUF> _segments;
	friend class Session;
};

struct RecvEvent : public IOCPEvent
{
public:
	RecvEvent() : IOCPEvent(EventType::Recv) {}
};

NAMESPACE_CLOSE;

