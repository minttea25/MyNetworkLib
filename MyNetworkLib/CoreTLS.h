#pragma once

NAMESPACE_OPEN(NetCore);
// extern thread_local instance definitions
extern thread_local uint32 TLS_Id;
extern thread_local SendBufferSPtr TLS_SendBuffer;


NAMESPACE_CLOSE;