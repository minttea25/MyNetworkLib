#include "pch.h"
#include "PacketHandler.h"

#include "fbs/Packet_generated.h"

using ubyte = unsigned char;

void PacketHandler::TestPacketHandler(const char* pkt, _Session& session)
{
	auto test_packet = Test::GetTestPacket(pkt);
	cout << "Msg: " << test_packet->msg()->c_str() << '\n';
	cout << "Number: " << test_packet->number() << '\n';
}
