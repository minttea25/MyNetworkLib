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
        ERR(errorCode, Failed to create new IO complete port);
        ASSERT_CRASH(_iocpHandle != INVALID_HANDLE_VALUE);
    }
    
}

NetCore::IOCPCore::~IOCPCore()
{
    bool suc = ::CloseHandle(_iocpHandle);
    if (suc == 0)
    {
        // Failed case
        WARN(ClosHandle was failed.);
    }
}

bool NetCore::IOCPCore::RegisterIOCP(IOCPObject* iocpObject)
{
    auto ret = ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle, 0, 0);
    if (ret == NULL)
    {
        auto errorCode = ::GetLastError();
        ERR(errorCode, Failed to CreateIoCompletionPort while registering iocp handle.);
        return false;
    }
    return true;
}

bool NetCore::IOCPCore::GetQueuedCompletionStatus(DWORD dwMilliseconds)
{
    DWORD numberOfBytesTransferred = 0;
    IOCPEvent* iocpEvent = nullptr;
    ULONG_PTR key = 0;

    // Atode GetQueuedCompletionStatusEx ni kaemasyo
    BOOL suc = ::GetQueuedCompletionStatus(_iocpHandle,
        OUT & numberOfBytesTransferred, OUT & key,
        OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), dwMilliseconds);

    if (suc == FALSE)
    {
        int32 errCode = ::GetLastError();
        switch (errCode)
        {
        case WAIT_TIMEOUT:
            return false;
        default:
            ERR(errCode, Failed to get queued completion status.);
            //iocpEvent->GetIOCPObjectRef()->Dispatch(iocpEvent, numberOfBytesTransferred);
            return false;
        }
    }

    iocpEvent->GetIOCPObjectRef()->Dispatch(iocpEvent, numberOfBytesTransferred);


    return true;
}
