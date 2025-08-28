#include "Network.hpp"
#include "app/Settings.hpp"
#include <cassert>

using namespace app::sys;
using namespace SLNet;

const SocketDescriptor Network::msc_ServerSocketDesc { app::Settings::GetInstance().GetSettings().at_path("app.server_port").value_or(uint16_t(8241)),
    app::Settings::GetInstance().GetSettings().at_path("app.server_addr").value_or("127.0.0.1") };

Network::Network()
    : m_RakPeer(RakPeerInterface::GetInstance())
{
    SocketDescriptor client {};
    m_RakPeer->Startup(1u, &client, 1u);
    const ConnectionAttemptResult result = m_RakPeer->Connect(msc_ServerSocketDesc.hostAddress, msc_ServerSocketDesc.port, nullptr, 0);
    assert(result == CONNECTION_ATTEMPT_STARTED);
}

Network::~Network()
{
    m_RakPeer->Shutdown(1000);
    RakPeerInterface::DestroyInstance(m_RakPeer);
}

void Network::Tick()
{
    m_MessageHandler.CleanUp();

    Packet* packet = nullptr;
    for (packet = m_RakPeer->Receive(); packet; packet = m_RakPeer->Receive()) {
        const InboundPacket ipacket(packet);
        m_MessageHandler.Publish(ipacket.GetOpcode(), ipacket);
    }
}
