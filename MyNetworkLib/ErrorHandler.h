#pragma once

// using glog later
// simple 

NAMESPACE_OPEN(NetCore);

// TODO : Change names later

/// <summary>
/// Application Error Codes
/// </summary>
enum Errors : DWORD
{
	NONE = 0,

	WSASTARTUP_FAILED = 101,
	WSAIOCTL_CONNECTEX_FAILED = 102,
	WSAIOCTL_ACCEPTEX_FAILED = 103,
	WSAClEANUP_FAILED = 104,
	WSASOCKETW_FAILED = 105,
	WSA_SET_LINGER_FAILED = 106,
	WSA_SET_REUSEADDR_FAILED = 107,
	WSA_SET_RCVBUF_FAILED = 108,
	WSA_SET_SNDBUF_FAILED = 109,
	WSA_SET_TCP_NODELAY_FAILED = 110,
	WSA_SET_UPDATE_ACCEPT_CONTEXT_FAILED = 111,
	WSA_BIND_FAILED = 112,
	WSA_BIND_ANYADDRESS_FAILED = 113,
	WSA_LISTEN_FAILED = 114,
	WSA_CLOSE_SOCKET_FAILED = 115,
	WSA_GET_PEER_NAME_FAILED = 116,
	WSA_SEND_FAILED = 117,
	WSA_RECV_FAILED = 118,
	WSA_CONNECTEX_FAILED = 119,
	WSA_ACCEPTEX_FAILED = 120,
	WSAIOCTL_DISCONNECTEX_FAILED = 121,
	WSA_DISCONNECTEX_FAILED = 122,

	THREAD_LOCK_TIMEOUT = 501,
	THREAD_LOCK_INVALID_WRITE_UNLOCK_ORDER = 502,
	THREAD_LOCK_MULTIPLE_READ_UNLOCK = 503,

	APP_REGISTER_HANDLE_FAILED_IOCP_CORE = 1001,

	APP_SERVICE_IOCPCORE_NULLPTR = 2001,
	APP_SERVICE_SESSIONFACTORY_NULLPTR = 2002,
	APP_SERVICE_SOCKADDR_INVALID = 2003,
	APP_SERVICE_IS_ALREADY_STARTED = 2004,
	APP_CLIENTSERVICE_CONNECT_FAILED = 2005,
	APP_SERVERSERVICE_LISTEN_FAILED = 2006,

	APP_CONNECTOR_SERVICE_WAS_NOT_CLIENT = 3001,

	APP_LISTENER_SERVICE_WAS_NOT_SERVER = 4001,

	APP_SESSION_SET_SOCKET_ALLOWED_ONLY_IN_CONNECTOR = 5001,

	APP_SENDBUFFER_SIZE_OVERFLOW = 6001,
	APP_RECVBUFFER_INVALID_DATA = 6002,
	APP_RECVBUFFER_OVERFLOW = 6003,

};

class ErrorHandler
{
public:
	static inline DWORD GetLastError()
	{
		return _app_last_error.load();
	}

	static inline void SetLastError(Errors error)
	{
		_app_last_error.store(error);
	}

	static inline void AssertCrash(const bool condition, const Errors errCode, const char* msg = nullptr)
	{
		if (!condition)
		{
			ERR_CODE_(errCode);
			ASSERT_CRASH(false);
		}
	}

	static inline void AssertCrash(const Errors errCode, const char* msg = nullptr)
	{
		ERR_CODE_(errCode);
		ASSERT_CRASH(false);
	}

	static inline DWORD CheckError(const bool condition, const Errors errCode)
	{
		if (!condition)
		{
			ERR_CODE_(errCode);
			return errCode;
		}
		return Errors::NONE;
	}

	static inline bool WSACheckSocketError(const int value, const Errors errCode, const bool crash = true)
	{
		if (value == SOCKET_ERROR)
		{
			const int32 wsaErr = ::WSAGetLastError();
			WSA_ERR(wsaErr);

			_app_last_error.store(errCode);

			if (crash)
			{
				// TODO : change later with other function
				std::cerr << "Last error was " << _app_last_error.load() << std::endl;
				ASSERT_CRASH("WSALastError was SOCKET_ERROR");
			}
			return false;
		}
		else return true;
	}

	static inline bool CheckSocketValid(const Socket& socket, const Errors errCode, const bool crash = true)
	{
		if (socket == INVALID_SOCKET)
		{
			const int32 wsaErr = ::WSAGetLastError();
			WSA_ERR(wsaErr);

			_app_last_error.store(errCode);

			if (crash)
			{
				// TODO : change later with other function
				std::cerr << "Last error was " << _app_last_error.load() << std::endl;
				ASSERT_CRASH("WSALastError was INVALID_SOCKET");
			}
			return false;
		}
		else return true;
	}

	static inline int32 WSACheckErrorExceptPending(const bool condition, DWORD errorCode)
	{
		if (condition == false)
		{
			const int32 err = ::WSAGetLastError();
			if (err == WSA_IO_PENDING) return Errors::NONE;
			else
			{
				_app_last_error.store(errorCode);
				ERR_CODE_(errorCode);
				return err;
			}
		}
		else return Errors::NONE;
	}

private:
	static Atomic<DWORD> _app_last_error;
};

template<typename T>
static inline void CheckError(T target, T expected, bool match, const std::string& msg, bool crash = true)
{
	if (equal(target, expected) != match)
	{
		DWORD errCode = ::GetLastError();
		ERR_CODE(errCode, msg);
		if (crash)
		{
			ASSERT_CRASH("Target was not expected value.");
		}
	}
}

template<typename T>
static inline void WSACheckError(T target, T expected, bool match, const std::string& msg, bool crash = true)
{
	if (equal(target, expected) != match)
	{
		int32 errCode = ::WSAGetLastError();
		ERR_CODE(errCode, msg);
		if (crash)
		{
			ASSERT_CRASH("Target was not expected value.");
		}
	}
}

NAMESPACE_CLOSE;