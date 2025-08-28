#pragma once
#include "slikenet/types.h"

namespace app {
struct InboundPacket
{
public:
    std::shared_ptr<const SLNet::Packet> m_packet;

public:
    InboundPacket() = default;
    explicit InboundPacket(SLNet::Packet* packet);

    ~InboundPacket() = default;

    SLNet::MessageID GetOpcode() const noexcept
    {
        return reinterpret_cast<SLNet::MessageID>(m_packet->data[0]);
    }

    std::string_view GetPayload() const noexcept
    {
        return std::string_view { reinterpret_cast<const char*>(m_packet->data + 1),
            m_packet->length - 1 };
    }
};

class OutboundPacket
{
public:
    template <typename ProtoType>
    static std::string Build(SLNet::MessageID id, const ProtoType& proto_msg)
    {
        std::string data;
        data += static_cast<unsigned char>(id);
        proto_msg.AppendToString(&data);

        return data;
    }
};
}
