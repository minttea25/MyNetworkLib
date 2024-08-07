#pragma once

NAMESPACE_OPEN(NetCore);
// extern thread_local instance definitions

/// <summary>
/// Identified id of thread used in TaskManager.
/// </summary>
extern thread_local uint32 TLS_Id;

/// <summary>
/// SendBuffer used in Socket IO
/// </summary>
extern thread_local SendBufferSPtr TLS_SendBuffer;

#ifdef USE_GLOBAL_JOB_SERIALIZER
/// <summary>
/// Indicate the current-executing job serializer in thread
/// </summary>
extern thread_local AJobSerializerSPtr TLS_CurrentJobSerializer;
/// <summary>
/// Each task executes jobs during this value (tick, ms)
/// </summary>
extern thread_local uint64 TLS_GlobalJobsExecutionMaxTickCount;
#endif

NAMESPACE_CLOSE;