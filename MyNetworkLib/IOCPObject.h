#pragma once

NAMESPACE_OPEN(NetCore);

class IOCPObject : public enable_shared_from_this<IOCPObject>
{
public:
	virtual HANDLE GetHandle() abstract;
public:
	virtual void Dispatch(struct IOCPEvent* event, int32 numberOfBytes = 0) abstract;
private:
};

NAMESPACE_CLOSE;
