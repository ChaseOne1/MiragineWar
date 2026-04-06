#include "Network.hpp"
#include "utility/ScriptManager.hpp"
#include "app/Settings.hpp"
#include "utility/MsgPack.hpp"
#include "MWProtocol/MessageIdentifiers.hh"

using namespace app::sys;
using namespace SLNet;

Network::Network()
    : m_RakPeer(RakPeerInterface::GetInstance())
    , m_LuaServices(utility::ScriptManager::GetLuaState().script(R"(return require("service"))"))
{
    SocketDescriptor client {};
    m_RakPeer->Startup(1u, &client, 1u);

    const ConnectionAttemptResult result = m_RakPeer->Connect(
        app::Settings::GetSystem()["app"]["server_addr"].get<std::string_view>().data(),
        app::Settings::GetSystem()["app"]["server_port"], nullptr, 0
        );
    assert(result == CONNECTION_ATTEMPT_STARTED);

    m_MessageHandler.Subscribe(proto::MessageID::ID_RPC_CALL,
        [](const InboundPacket& packet) { GetInstance().HandleLuaRpcCall(packet); });

    sol::table network = utility::ScriptManager::GetLuaState().script(R"(return require("builtin.network"))");
    network["call"] = LuaRpcCall;
    network["get_average_ping"] = []() { Network::GetInstance().m_RakPeer->GetAveragePing(UNASSIGNED_SYSTEM_ADDRESS); };
    network["get_last_ping"] = []() { Network::GetInstance().m_RakPeer->GetLastPing(UNASSIGNED_SYSTEM_ADDRESS); };
}

Network::~Network()
{
    m_RakPeer->Shutdown(1000);
    RakPeerInterface::DestroyInstance(m_RakPeer);
}

void Network::Tick()
{
    Packet* packet = nullptr;
    for (packet = m_RakPeer->Receive(); packet; packet = m_RakPeer->Receive()) {
        const InboundPacket ipacket(packet);
        SLNet::MessageID opcode = ipacket.GetOpcode();
        switch (opcode) {
        case ID_TIMESTAMP:
            if (ipacket.GetPayload().length() <= sizeof(SLNet::Time)) break;
            opcode = ipacket.GetPayload().substr(sizeof(SLNet::Time), sizeof(SLNet::MessageID))[0];
            break;
        default:
            break;
        }
        m_MessageHandler.Publish(opcode, ipacket);
    }
}

void Network::LuaRpcCall(lua_Integer service_id, lua_Integer interface_id,
        PacketPriority priority, PacketReliability reliability, uint8_t channel,
        sol::variadic_args args)
{
    std::string data(1, proto::MessageID::ID_RPC_CALL);
    utility::mp::encode::integer(data, service_id);
    utility::mp::encode::integer(data, interface_id);
    utility::mp::pack(data, args);
    GetInstance().GetRakPeerInterface()->Send(data.data(), data.size(),
        priority, reliability, channel, UNASSIGNED_RAKNET_GUID, true);
}

void Network::HandleLuaRpcCall(const InboundPacket& packet)
{
    std::string_view payload = packet.GetPayload();
    const lua_Integer service_id = utility::mp::decode::integer(payload);
    const lua_Integer interface_id = utility::mp::decode::integer(payload);

    const sol::table service_mapping = GetInstance().m_LuaServices["mapping"].get<sol::table>();
    const sol::optional<sol::table> service = service_mapping.get<sol::optional<sol::table>>(service_id);
    if (!service) {
        LOGE("The service of RPC({}:{}) has not been setup", service_id, interface_id);
        return;
    }

    const sol::optional<sol::string_view> interface_name
        = service->traverse_get<sol::optional<sol::string_view>>("mapping", interface_id);
    if (!interface_name) {
        LOGE("No such interface name matches of RPC({}:{})", service_id, interface_id);
        return;
    }

    // fuck mircosoft, the "interface" has already been aliased in windows.h
    const sol::optional<sol::function> interfc = service->traverse_get<sol::optional<sol::function>>("client", *interface_name);
    if (!interfc) {
        LOGE("No such interface matches the RPC({}:{})", service_id, interface_id);
        return;
    }

    (*interfc)(sol::as_args(utility::mp::unpack(payload, utility::ScriptManager::GetLuaState())));
}
