#pragma once

NAMESPACE_OPEN(NetCore);

/// <summary>
/// Helper funtion for ::setsockopt.
/// </summary>
/// <typeparam name="T">Option type</typeparam>
/// <param name="socket">Socket</param>
/// <param name="level">Socket level</param>
/// <param name="optname">Option name</param>
/// <param name="optval">Option value</param>
/// <returns>Same as return value of ::setsockopt.</returns>
template<typename T>
static inline int32 SetSockOpt(Socket socket, int32 level, int32 optname, T optval)
{
	return ::setsockopt(socket, level, optname, reinterpret_cast<const char*>(&optval), sizeof(T));
}

/// <summary>
/// SocketUtils is a class provides static methods for sockets.
/// </summary>
class SocketUtils
{
public:
	/// <summary>
	/// WSAData will start up here. It must be called at first.
	/// </summary>
	static void Init();
	/// <summary>
	/// Cleanup WSA.
	/// </summary>
	static void Clear();
	
public:
	/// <summary>
	/// Set operating parameters associated with socket. Helper function of ::WSAIoctl.
	/// </summary>
	/// <param name="socket">A descriptor identfying a socket.</param>
	/// <param name="ioControlCode">The control code of operation to perform.</param>
	/// <param name="guid">A pointer to the input buffer.</param>
	/// <param name="fn">A pointer to the output buffer.</param>
	/// <param name="overlapped">A pointer to a WSAOVERLAPPED strucure.</param>
	/// <param name="completionRoutine">Overlapped completion routine.</param>
	/// <returns></returns>
	static int32 WSAIoctlHelper(Socket socket, DWORD ioControlCode, GUID guid, LPVOID* fn, LPWSAOVERLAPPED overlapped = NULL, LPWSAOVERLAPPED_COMPLETION_ROUTINE completionRoutine = NULL)
	{
		// See: https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsaioctl

		NOT_USE DWORD bytesReturned = 0;
		return ::WSAIoctl(socket, ioControlCode, &guid, sizeof(guid),
			fn, sizeof(fn), OUT & bytesReturned, overlapped, completionRoutine);
	}

	/// <summary>
	/// Create TCP socket using Overlapped.
	/// </summary>
	static Socket CreateSocket()
	{
		// See: https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsasocketw#return-value
		
		Socket socket = ::WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
		ErrorHandler::CheckSocketValid(socket, Errors::WSASOCKETW_FAILED);
		return socket;
	}

	/// <summary>
	/// Set linger option at socket.
	/// </summary>
	/// <param name="socket">socket</param>
	/// <param name="onoff">linger option on/off option</param>
	/// <param name="linger">linger time</param>
	/// <returns>true if setsockopt is successful, false otherwise.</returns>
	static bool SetLinger(Socket socket, uint16 onoff, uint16 linger)
	{
		LINGER lingerOption {};
		lingerOption.l_onoff = onoff;
		lingerOption.l_linger = linger;
		
		const int32 res = SetSockOpt(socket, SOL_SOCKET, SO_LINGER, lingerOption);
		return ErrorHandler::WSACheckSocketError(res, Errors::WSA_SET_LINGER_FAILED, false);
	}

	/// <summary>
	/// Set reuse address option at socket.
	/// </summary>
	/// <param name="socket">socket</param>
	/// <param name="flag">option flag</param>
	/// <returns>true if setsockopt is successful, false otherwise.</returns>
	static bool SetReuseAddress(Socket socket, bool flag)
	{
		const int32 res = SetSockOpt(socket, SOL_SOCKET, SO_REUSEADDR, flag);
		return ErrorHandler::WSACheckSocketError(res, Errors::WSA_SET_REUSEADDR_FAILED, false);
	}

	/// <summary>
	/// Set recv buffer size at socket.
	/// </summary>
	/// <param name="socket">socket</param>
	/// <param name="size">buffer size</param>
	/// <returns>true if setsockopt is successful, false otherwise.</returns>
	static bool SetRecvBufferSize(Socket socket, int32 size)
	{
		const int32 res = SetSockOpt(socket, SOL_SOCKET, SO_RCVBUF, size);
		return ErrorHandler::WSACheckSocketError(res, Errors::WSA_SET_RCVBUF_FAILED, false);
	}

	/// <summary>
	/// Set send buffer size at socket.
	/// </summary>
	/// <param name="socket">socket</param>
	/// <param name="size">buffer size</param>
	/// <returns>true if setsockopt is successful, false otherwise.</returns>
	static bool SetSendBufferSize(Socket socket, int32 size)
	{
		const int32 res = SetSockOpt(socket, SOL_SOCKET, SO_SNDBUF, size);
		return ErrorHandler::WSACheckSocketError(res, Errors::WSA_SET_SNDBUF_FAILED, false);
	}

	/// <summary>
	/// Set tcp no delay option at socket.
	/// </summary>
	/// <param name="socket">socket</param>
	/// <param name="flag">option flag</param>
	/// <returns>true if setsockopt is successful, false otherwise.</returns>
	static bool SetTcpNoDelay(Socket socket, bool flag)
	{
		const int32 res = SetSockOpt(socket, SOL_SOCKET, TCP_NODELAY, flag);
		return ErrorHandler::WSACheckSocketError(res, Errors::WSA_SET_TCP_NODELAY_FAILED, false);
	}

	/// <summary>
	/// Set listen socket option to accept socket.
	/// </summary>
	/// <param name="socket">accept socket</param>
	/// <param name="listenSocket">listen socket</param>
	/// <returns>true if setsockopt is successful, false otherwise.</returns>
	static bool SetUpdateAcceptSocket(Socket socket, Socket listenSocket)
	{
		const int32 res = SetSockOpt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, listenSocket);
		return ErrorHandler::WSACheckSocketError(res, Errors::WSA_SET_UPDATE_ACCEPT_CONTEXT_FAILED, false);
	}

	/// <summary>
	/// Bind socket with socket address.
	/// </summary>
	/// <param name="listenSocket">listen socket to bind</param>
	/// <param name="sockaddr">endpoint address</param>
	/// <returns>true if bind is successful, false otherwise.</returns>
	static bool Bind(Socket listenSocket, SOCKADDR_IN* sockaddr)
	{
		const int32 res = ::bind(listenSocket, reinterpret_cast<const SOCKADDR*>(sockaddr), sizeof(SOCKADDR_IN));
		return ErrorHandler::WSACheckSocketError(res, Errors::WSA_BIND_FAILED);
	}

	/// <summary>
	/// Bind socket with local socket address. It is used for ConnectEx.
	/// </summary>
	/// <param name="socket">socket to bind</param>
	/// <param name="port">port</param>
	/// <returns>true if bind is successful, false otherwise.</returns>
	static bool BindAnyAddress(Socket socket, uint16 port = 0)
	{
		/*SOCKADDR_IN myAddress{};
		myAddress.sin_family = AF_INET;
		myAddress.sin_addr.s_addr = ::htonl(INADDR_ANY);
		myAddress.sin_port = ::htons(port);*/
		SOCKADDR_IN myAddress = AddrUtils::GetMyAddress();

		const int32 res = ::bind(socket, reinterpret_cast<const SOCKADDR*>(&myAddress), sizeof(myAddress));
		return ErrorHandler::WSACheckSocketError(res, Errors::WSA_BIND_ANYADDRESS_FAILED);
	}

	/// <summary>
	/// Set socket to listen.
	/// </summary>
	/// <param name="listenSocket">socket to listen</param>
	/// <param name="backlog">backlog</param>
	/// <returns>true if listen is successful, false otherwise.</returns>
	static bool Listen(Socket listenSocket, int32 backlog = SOMAXCONN)
	{
		const int32 res = ::listen(listenSocket, backlog);
		return ErrorHandler::WSACheckSocketError(res, Errors::WSA_LISTEN_FAILED);
	}

	/// <summary>
	/// Close a socket.
	/// </summary>
	/// <param name="socket">socket to close</param>
	static void Close(Socket& socket)
	{
		if (socket != INVALID_SOCKET)
		{
			const int32 res = ::closesocket(socket);
			ErrorHandler::WSACheckSocketError(res, Errors::WSA_CLOSE_SOCKET_FAILED);
		}
		socket = INVALID_SOCKET;
	}
public:
	/// <summary>
	/// ConnectEx pointer to register at runtime.
	/// </summary>
	static LPFN_CONNECTEX ConnectEx;
	/// <summary>
	/// AcceptEx pointer to register at runtime.
	/// </summary>
	static LPFN_ACCEPTEX AcceptEx;
	/// <summary>
	/// Disconnect pointer to register at runtime.
	/// </summary>
	static LPFN_DISCONNECTEX DisconnectEx;

private:
	static void _load_fn_pointers();
};

NAMESPACE_CLOSE;