#include "pch.h"
#include "CoreTLS.h"

NAMESPACE_OPEN(NetCore);

/// <summary>
/// Unique id of task created in TaskManager
/// </summary>
thread_local uint32 TLS_Id = UNSIGNED_INVALID;
/// <summary>
/// SendBuffer used in Socket IO
/// </summary>
thread_local SendBufferSPtr TLS_SendBuffer = nullptr;

#ifdef USE_GLOBAL_JOB_SERIALIZER
/// <summary>
/// Indicate the current-executing job serializer in thread
/// </summary>
thread_local AJobSerializerSPtr TLS_CurrentJobSerializer = nullptr;
/// <summary>
/// Each task executes jobs during this value (tick, ms)
/// </summary>
thread_local uint64 TLS_GlobalJobsExecutionMaxTickCount = 0;
#endif

NAMESPACE_CLOSE;