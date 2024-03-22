#include "pch.h"
#include "IOCPCore.h"
#include "IOCPObject.h"

NetCore::IOCPCore::IOCPCore()
{
    // Create new handle
    _iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 
        nullptr, 0, 0);
    if (_iocpHandle == NULL)
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

bool NetCore::IOCPCore::CreateIOCP(IOCPObject* iocpObject)
{
    ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle, 0, 0);

    return false;
}

bool NetCore::IOCPCore::GetQueuedCompletionStatus(DWORD dwMilliseconds)
{
    DWORD numberOfBytesTransferred = 0;
    IOCPEvent* iocpEvent = nullptr;

    // Atode GetQueuedCompletionStatusEx ni kaemasyo
    BOOL suc = ::GetQueuedCompletionStatus(_iocpHandle,
        OUT & numberOfBytesTransferred, OUT & _completeionKey,
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
            return false;
        }
    }

    IOCPObject* iocpObject = iocpEvent->GetIOCPObjectRef();
    iocpObject->Dispatch(iocpEvent, numberOfBytesTransferred);


    return true;
}
