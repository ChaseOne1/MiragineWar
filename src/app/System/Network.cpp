#include "Network.hpp"
#include "app/ScriptManager.hpp"
#include "app/Settings.hpp"
#include "utility/MsgPack.hpp"

using namespace app::sys;
using namespace SLNet;

Network::Network()
    : m_RakPeer(RakPeerInterface::GetInstance())
{
    SocketDescriptor client {};
    m_RakPeer->Startup(1u, &client, 1u);

    const ConnectionAttemptResult result = m_RakPeer->Connect(
        app::Settings::GetSettings()["app"]["server_addr"].get<std::string_view>().data(),
        app::Settings::GetSettings()["app"]["server_port"], nullptr, 0
        );
    assert(result == CONNECTION_ATTEMPT_STARTED);

    m_MessageHandler.Subscribe(Network::MessageID::ID_RPC_CALL,
        [](const InboundPacket& packet) { GetInstance().HandleLuaRpcCall(packet); });

    auto type = app::ScriptManager::GetLuaState().new_usertype<Network>(
        "Network", sol::no_constructor);
    type["call"] = LuaRpcCall;

    m_LuaServices = app::ScriptManager::GetLuaState().script(R"(return require("service"))");
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

void Network::LuaRpcCall(lua_Integer service_id, lua_Integer interface_id,
        PacketPriority priority, PacketReliability reliability, uint8_t channel,
        sol::variadic_args args)
{
    std::string data(1, ID_RPC_CALL);
    utility::mp::encode::integer(data, service_id);
    utility::mp::encode::integer(data, interface_id);
    utility::mp::pack(data, args);
    GetInstance().GetRakPeerInterface()->Send(data.data(), data.size(),
        priority, reliability, channel, UNASSIGNED_RAKNET_GUID, false);
}

void Network::HandleLuaRpcCall(const InboundPacket& packet)
{
    std::string_view payload = packet.GetPayload();
    const lua_Integer service_id = utility::mp::decode::integer(payload);
    const lua_Integer interface_id = utility::mp::decode::integer(payload);

    const sol::table service_mapping = GetInstance().m_LuaServices["mapping"].get<sol::table>();
    const sol::table service = service_mapping[service_id].get_or<sol::table>(sol::nil);
    if (!service.valid()) {
        SDL_Log("app::sys::Network::HandleLuaRpcCall:"
                "The service of RPC(%lld:%lld) has not been setup",
            service_id, interface_id);
        return;
    }

    const sol::table interface_mapping = service["mapping"].get_or<sol::table>(sol::nil);
    if (!interface_mapping.valid()) {
        SDL_Log("app::sys::Network::HandleLuaRpcCall:"
                "The interface mapping of RPC(%lld:%lld) service has not been set",
            service_id, interface_id);
        return;
    }

    // fuck mircosoft, the "interface" has already been aliased in windows.h
    const sol::function interfc = service[interface_id].get_or<sol::function>(sol::nil);
    if (!interfc.valid()) {
        SDL_Log("app::sys::Network::HandleLuaRpcCall:"
                "No such interface matches the RPC(%lld:%lld)",
            service_id, interface_id);
        return;
    }

    interfc(sol::as_args(utility::mp::unpack(payload, ScriptManager::GetLuaState())));
}
