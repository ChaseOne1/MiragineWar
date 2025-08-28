#include "NetPacket.hpp"
#include "app/System/Network.hpp"

using namespace app;

InboundPacket::InboundPacket(SLNet::Packet* packet)
    : m_packet(packet, [](SLNet::Packet* packet) { app::sys::Network::GetRakPeerInterface()->DeallocatePacket(packet); })
{ }
