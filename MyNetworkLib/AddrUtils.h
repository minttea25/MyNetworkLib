#pragma once

NAMESPACE_OPEN(NetCore);

class AddrUtils
{
public:
	static constexpr ushort ANY_PORT = 0;
	static inline SOCKADDR_IN GetTcpAddress(PCSTR ip, ushort port)
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
	static inline SOCKADDR_IN GetMyAddress(ushort port = ANY_PORT)
	{
		SOCKADDR_IN myAddress{};
		memset(&myAddress, 0, sizeof(myAddress));
		myAddress.sin_family = AF_INET;
		myAddress.sin_addr.s_addr = ::htonl(INADDR_ANY);
		myAddress.sin_port = ::htons(port);
		return myAddress;
	}
};

NAMESPACE_CLOSE;