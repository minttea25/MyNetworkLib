#include "pch.h"
#include "CoreTLS.h"

NAMESPACE_OPEN(NetCore);

thread_local uint32 TLS_Id = UNSIGNED_INVALID;
thread_local SendBufferSPtr TLS_SendBuffer = nullptr;

NAMESPACE_CLOSE;