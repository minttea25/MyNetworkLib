#pragma once

NAMESPACE_OPEN(NetCore);

class AddrUtils
{
public:
	static constexpr ushort ANY_PORT = 0;
	static constexpr auto MAX_PORT = 65535;
	static inline SOCKADDR_IN GetTcpAddress(const PCSTR ip, const ushort port)
	{
		SOCKADDR_IN addr{};
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		::inet_pton(AF_INET, ip, &addr.sin_addr);
		addr.sin_port = ::htons(port);
		return addr;
	}

	/// <summary>
	/// Get my address with port. It is used for ConnectEx fn.
	/// </summary>
	/// <param name="port">port number; default: 0 (any port)</param>
	/// <returns>address</returns>
	static inline SOCKADDR_IN GetMyAddress(const ushort port = ANY_PORT)
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