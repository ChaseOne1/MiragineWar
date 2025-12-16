#include "Network.hpp"
#include "app/Settings.hpp"

using namespace app::sys;
using namespace SLNet;

Network::Network()
    : m_RakPeer(RakPeerInterface::GetInstance())
{
    SocketDescriptor client {};
    m_RakPeer->Startup(1u, &client, 1u);

    const ConnectionAttemptResult result = m_RakPeer->Connect(
        app::Settings::GetSettings()["app"]["server_addr"].get<std::string>().c_str(),
        app::Settings::GetSettings()["app"]["server_port"], nullptr, 0
        );
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

void Network::RegisterEnv(sol::environment& env)
{
    env.new_usertype<app::InboundPacket>(
        "InboundPacket", sol::no_constructor,
        "GetOpcode", &app::InboundPacket::GetOpcode,
        "GetPayload", &app::InboundPacket::GetPayload
        );

    //TODO: register outbound packet

    env.new_usertype<Network>(
        "Network", sol::no_constructor,
        "subscribe", &Network::Subscribe<Handler>,
        "unsubscribe", &Network::Unsubscribe
        );
}
