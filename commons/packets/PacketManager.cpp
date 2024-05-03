#include "pch.h"
#include "PacketManager.h"

using ubyte = unsigned char;

PacketManager* GPacketManager = nullptr;

void PacketManager::_init()
{
	// add

	_handlers.insert({ 1, PacketHandler::TestPacketHandler });
}
