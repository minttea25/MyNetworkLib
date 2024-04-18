#include "pch.h"
#include "IOCPCore.h"
#include "IOCPObject.h"

NetCore::IOCPCore::IOCPCore()
{
    // Create new handle
    _iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 
        nullptr, 0, 0);
    if (_iocpHandle == INVALID_HANDLE_VALUE)
    {
        int32 errorCode = ::GetLastError();
        ERR_CODE(errorCode, Failed to create new IO complete port);
        ASSERT_CRASH(_iocpHandle != INVALID_HANDLE_VALUE);
    }
    
}

NetCore::IOCPCore::~IOCPCore()
{
    DESTRUCTOR(~IOCPCore);

    bool suc = ::CloseHandle(_iocpHandle);
    if (suc == 0)
    {
        // Failed case
        WARN(ClosHandle was failed.);
    }
}

bool NetCore::IOCPCore::RegisterHandle(IOCPObjectSPtr iocpObject)
{
    auto res = ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle, 0, 0);
    return ErrorHandler::CheckError(res != NULL, Errors::APP_REGISTER_HANDLE_FAILED_IOCP_CORE) == Errors::NONE;
}

bool NetCore::IOCPCore::ProcessQueuedCompletionStatus(DWORD dwTimeoutMilliseconds)
{
    DWORD numberOfBytesTransferred = 0;
    IOCPEvent* iocpEvent = nullptr;
    ULONG_PTR key = 0;

    // Atode GetQueuedCompletionStatusEx ni kaemasyo
    BOOL suc = ::GetQueuedCompletionStatus(_iocpHandle,
        OUT & numberOfBytesTransferred, OUT & key,
        OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), dwTimeoutMilliseconds);

    DWORD errCode = ::GetLastError();
    std::cout << errCode << std::endl;
    if (iocpEvent != nullptr) std::cout << (int)(iocpEvent->GetEventType()) << std::endl;

    if (suc == FALSE)
    {
        DWORD errCode = ::GetLastError();
        switch (errCode)
        {
        case WAIT_TIMEOUT:
            return false;
        default:
            ERR_CODE(errCode, errcode was not wait_timeout);
            // iocpEvent->GetIOCPObjectRef().lock()->Process(iocpEvent, numberOfBytesTransferred);
            return false;
        }
    }
    else iocpEvent->GetIOCPObjectWPtr().lock()->Process(iocpEvent, numberOfBytesTransferred);

    return true;
}
