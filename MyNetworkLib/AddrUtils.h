#pragma once

NAMESPACE_OPEN(NetCore);

class AddrUtils
{
public:
	static constexpr uint16 ANY_PORT = 0;
	static constexpr auto MAX_PORT = 65535;

	static inline SOCKADDR_IN GetTcpAddress(const wchar_t* ip, const uint16 port)
	{
		SOCKADDR_IN addr {};
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		::InetPton(AF_INET, ip, &addr.sin_addr);
		addr.sin_port = ::htons(port);
		return addr;
	}

	static inline SOCKADDR_IN GetTcpAddress(const std::wstring ip, const uint16 port)
	{
		SOCKADDR_IN addr{};
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		::InetPton(AF_INET, ip.c_str(), &addr.sin_addr);
		addr.sin_port = ::htons(port);
		return addr;
	}

	static inline bool SetTcpAddress(const PCWSTR ip, const uint16 port, PSOCKADDR_IN paddr)
	{
		memset(paddr, 0, sizeof(SOCKADDR_IN));
		paddr->sin_family = AF_INET;
		paddr->sin_port = ::htons(port);
		if (::InetPtonW(AF_INET, ip, &paddr->sin_addr) <= 0) return false;
		else return true;
	}

	static inline bool GetSockName(const Socket socket, OUT uint16* port, OUT wchar_t* localAddr, const int32 localAddrLen)
	{
		SOCKADDR_IN addr{};
		int32 addrLen = sizeof(addr);
		if (::getsockname(socket, (SOCKADDR*)&addr, &addrLen) == 0)
		{
			::InetNtopW(AF_INET, &addr.sin_addr, localAddr, localAddrLen);
			*port = ::ntohs(addr.sin_port);
			return true;
		}
		else
		{
			return false;
		}
	}

	static inline bool GetPeerName(const Socket socket, OUT uint16* port, OUT wchar_t* remoteAddr, const int32 remoteAddrLen)
	{
		SOCKADDR_IN addr{};
		int32 addrLen = sizeof(addr);
		if (::getpeername(socket, (SOCKADDR*)&addr, &addrLen) == 0)
		{
			::InetNtopW(AF_INET, &addr.sin_addr, remoteAddr, remoteAddrLen);
			*port = ntohs(addr.sin_port);
			return true;
		}
		else
		{
			return false;
		}
	}

	/// <summary>
	/// Get my address with port. It is used for ConnectEx fn.
	/// </summary>
	/// <param name="port">port number; default: 0 (any port)</param>
	/// <returns>address</returns>
	static inline SOCKADDR_IN GetMyAddress(const uint16 port = ANY_PORT)
	{
		SOCKADDR_IN myAddress{};
		memset(&myAddress, 0, sizeof(myAddress));
		myAddress.sin_family = AF_INET;
		myAddress.sin_addr.s_addr = ::htonl(INADDR_ANY);
		myAddress.sin_port = ::htons(port);
		return myAddress;
	}

	static inline bool IsValid(const SOCKADDR_IN& addr)
	{
		// Address family must be AF_INET
		if (addr.sin_family != AF_INET)
			return false;

		// Port number must be within the valid range (0-65535)
		if (addr.sin_port == 0 || addr.sin_port > MAX_PORT)
			return false;

		return true;
	}
};

NAMESPACE_CLOSE;