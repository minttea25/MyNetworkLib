#include "pch.h"
#include "SocketUtils.h"

LPFN_CONNECTEX NetCore::SocketUtils::ConnectEx = nullptr;
LPFN_ACCEPTEX NetCore::SocketUtils::AcceptEx = nullptr;

void NetCore::SocketUtils::Init()
{
	// See: https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsastartup

	WSADATA wsaData{};
	int32 res = ::WSAStartup(MAKEWORD(2, 2), OUT & wsaData);
	ErrorHandler::WSACheckSocketError(res, Errors::WSASTARTUP_FAILED);

	_load_fn_pointers();
}

void NetCore::SocketUtils::Clear()
{
	const int32 res = ::WSACleanup();
	ErrorHandler::WSACheckSocketError(res, Errors::WSAClEANUP_FAILED, false);
}

void NetCore::SocketUtils::_load_fn_pointers()
{
	Socket s = CreateSocket();

	int32 res = -1;

	res = WSAIoctlHelper(s, SIO_GET_EXTENSION_FUNCTION_POINTER, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&ConnectEx));
	ErrorHandler::WSACheckSocketError(res, Errors::WSAIOCTL_CONNECTEX_FAILED);

	res = WSAIoctlHelper(s, SIO_GET_EXTENSION_FUNCTION_POINTER, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&AcceptEx));
	ErrorHandler::WSACheckSocketError(res, Errors::WSAIOCTL_ACCEPTEX_FAILED);

	Close(s);
}


