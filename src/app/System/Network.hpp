#pragma once
#include "slikenet/peerinterface.h"
#include "slikenet/MessageIdentifiers.h"
#include "slikenet/types.h"
#include "utility/Topics.hpp"

namespace app {
    class InboundPacket;
}

namespace app::sys {
class Network : public utility::Singleton<Network>
{
    friend class utility::Singleton<Network>;

    using Handler = std::function<void(const InboundPacket&)>;

private:
    SLNet::RakPeerInterface* m_RakPeer;

    utility::Topics<SLNet::MessageID, Handler> m_MessageHandler;

    sol::table m_LuaServices;

private:

    static void LuaRpcCall(lua_Integer, lua_Integer, PacketPriority, PacketReliability, uint8_t, sol::variadic_args);
    static void HandleLuaRpcCall(const InboundPacket&);

public:
    enum MessageID : uint8_t
{
    ID_RPC_CALL = ID_USER_PACKET_ENUM,
};

public:
    Network();
    ~Network();

    void Tick();

    template <typename Callback>
    static auto Subscribe(SLNet::MessageID msg, Callback&& callback)
    {
        return GetInstance().m_MessageHandler.Subscribe(msg, std::forward<Callback>(callback));
    }

    static auto GetNextSubscriberID() noexcept
    {
        return GetInstance().m_MessageHandler.GetNextSubscriberID();
    }

    static void Unsubscribe(SLNet::MessageID msg, utility::TopicsSubscriberID id)
    {
        GetInstance().m_MessageHandler.Unsubscribe(msg, id);
    }

    static SLNet::RakPeerInterface* GetRakPeerInterface()
    {
        return GetInstance().m_RakPeer;
    }
};
}

namespace app {
class InboundPacket
{
public:
    std::shared_ptr<const SLNet::Packet> m_Packet;

public:
    explicit InboundPacket(SLNet::Packet* packet)
        : m_Packet(packet,
              [](SLNet::Packet* packet) { app::sys::Network::GetRakPeerInterface()->DeallocatePacket(packet); })
    { }

    ~InboundPacket() = default;

    SLNet::MessageID GetOpcode() const noexcept
    {
        return reinterpret_cast<SLNet::MessageID>(m_Packet->data[0]);
    }

    std::string_view GetPayload() const noexcept
    {
        return std::string_view { reinterpret_cast<const char*>(m_Packet->data + 1),
            m_Packet->length - 1 };
    }
};

class OutboundPacket
{
};

}
