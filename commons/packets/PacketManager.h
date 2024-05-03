#pragma once

#ifdef SERVER
class ClientSession;
using _Session = ClientSession;
#endif // SERVER

#ifdef CLIENT
class ServerSession;
using _Session = ServerSession;
#endif // CLIENT

using ushort = unsigned __int16;

class PacketManager
{
public:
	PacketManager() { _init(); }
	PacketManager(const PacketManager&) = delete; // copy
	PacketManager(PacketManager&&) = delete; // move

	inline void Dispatch(const ushort id, const char* ptr, _Session& session)
	{
		auto it = _handlers.find(id);
		if (it != _handlers.end()) it->second(ptr, session);
		else
		{
			std::cout << "No registered id: " << id << std::endl;
		}
	}
private:
	void _init();

private:
	std::map<ushort, void(*)(const char*, _Session&)> _handlers;
};

extern class PacketManager* GPacketManager;

