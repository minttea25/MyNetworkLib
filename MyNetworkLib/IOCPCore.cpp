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
        WARN(CloseHandle was failed.);
    }
}

bool NetCore::IOCPCore::RegisterHandle(IOCPObjectSPtr iocpObject)
{
    auto res = ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle, 0, 0);
    return ErrorHandler::CheckError(res != NULL, Errors::APP_REGISTER_HANDLE_FAILED_IOCP_CORE) == Errors::NONE;
}

bool NetCore::IOCPCore::ProcessQueuedCompletionStatusEx(DWORD dwTimeoutMilliseconds)
{
    OVERLAPPED_ENTRY events[MAX_ENTRY_COUNT];
    DWORD numberOfEntriesTransferred = 0;
    BOOL alertable = false;

    auto suc = ::GetQueuedCompletionStatusEx(
        _iocpHandle, 
        OUT events, MAX_ENTRY_COUNT, OUT &numberOfEntriesTransferred,
        dwTimeoutMilliseconds, ALERTABLE);

    if (suc == FALSE)
    {
        DWORD errCode = ::GetLastError();
        switch (errCode)
        {
        case WAIT_TIMEOUT:
            return false;
        default:
            ERR_CODE(errCode, errcode was not wait_timeout);
            return false;
        }
    }
    else
    {
        PRINT(NumberOfEntriesTransferred: , numberOfEntriesTransferred);
        for (DWORD i = 0; i < numberOfEntriesTransferred; ++i)
        {
            OVERLAPPED_ENTRY* entry = &events[i];
            LPOVERLAPPED overlapped = entry->lpOverlapped;
            DWORD numberOfBytesTransferred = entry->dwNumberOfBytesTransferred;
            IOCPEvent* iocpEvent = reinterpret_cast<IOCPEvent*>(overlapped);
            iocpEvent->GetIOCPObjectWPtr().lock()->Process(iocpEvent, numberOfBytesTransferred);
        }
    }

    return true;
}

bool NetCore::IOCPCore::ProcessQueuedCompletionStatus(DWORD dwTimeoutMilliseconds)
{
    DWORD numberOfBytesTransferred = 0;
    IOCPEvent* iocpEvent = nullptr;
    ULONG_PTR key = 0;

    BOOL suc = ::GetQueuedCompletionStatus(_iocpHandle,
        OUT & numberOfBytesTransferred, OUT & key,
        OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), dwTimeoutMilliseconds);

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
    else
    {
        iocpEvent->GetIOCPObjectWPtr().lock()->Process(iocpEvent, numberOfBytesTransferred);
    }
}
