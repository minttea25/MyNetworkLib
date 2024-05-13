#pragma once

NAMESPACE_OPEN(NetCore);
// extern thread_local instance definitions

/// <summary>
/// Identified id of thread used in TaskManager.
/// </summary>
extern thread_local uint32 TLS_Id;

/// <summary>
/// Each thread has a SendBuffer for Socket IO.
/// </summary>
extern thread_local SendBufferSPtr TLS_SendBuffer;

NAMESPACE_CLOSE;