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

	APP_REGISTER_HANDLE_FAILED_IOCP_CORE = 1001,
};

class ErrorHandler
{
public:
	static inline DWORD GetLastError()
	{
		return _error.load();
	}

	static inline bool WSACheckSocketError(const int value, const Errors errCode, const bool crash = true)
	{
		if (value == SOCKET_ERROR)
		{
			const int32 wsaErr = ::WSAGetLastError();
			WSA_ERR(wsaErr);

			_error.store(errCode);

			if (crash)
			{
				// TODO : change later with other function
				std::cerr << "Last error was " << _error.load() << std::endl;
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

			_error.store(errCode);

			if (crash)
			{
				// TODO : change later with other function
				std::cerr << "Last error was " << _error.load() << std::endl;
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
				_error.store(errorCode);
				return err;
			}
		}
		else return Errors::NONE;
	}

	static inline DWORD CheckError(const bool condition, DWORD errorCode)
	{
		if (condition == false)
		{
			const DWORD err = ::GetLastError();
			if (err == WSA_IO_PENDING) return Errors::NONE;
			else
			{
				_error.store(errorCode);
				return err;
			}
		}
		else return Errors::NONE;
	}

	static inline void Warn(const std::string& msg)
	{
		WARN(msg);
	}

	static inline void Err(const std::string& msg)
	{
		ERR(msg);
	}

	static inline void Err(const std::string& msg, const DWORD code)
	{
		ERR_CODE(code, msg);
	}

	static inline void Err(const std::string& msg, const int32 code)
	{
		ERR_CODE(code, msg);
	}

private:
	static Atomic<DWORD> _error;
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