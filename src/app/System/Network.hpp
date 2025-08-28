#pragma once
#include "slikenet/peerinterface.h"
#include "slikenet/MessageIdentifiers.h"
#include "slikenet/types.h"
#include "utility/Topics.hpp"
#include "app/NetPacket.hpp"

namespace app::sys {
class Network : public utility::Singleton<Network>
{
    friend class utility::Singleton<Network>;

    using Handler = std::function<void(const InboundPacket&)>;

private:
    static const SLNet::SocketDescriptor msc_ServerSocketDesc;

    SLNet::RakPeerInterface* m_RakPeer;

    utility::Topics<SLNet::MessageID, Handler> m_MessageHandler;

public:
    Network();
    ~Network();

    void Tick();

    template <typename Callback>
    static auto Subcribe(SLNet::MessageID msg, Callback&& callback)
    {
        return GetInstance().m_MessageHandler.Subscribe(msg, std::forward<Callback>(callback));
    }

    static auto GetNextSubscriberID() noexcept
    {
        return GetInstance().m_MessageHandler.GetNextSubscriberID();
    }

    static void UnSubscribe(SLNet::MessageID msg, utility::TopicsSubscriberID id)
    {
        GetInstance().m_MessageHandler.Unsubscribe(msg, id);
    }

    static auto GetRakPeerInterface()
    {
        return GetInstance().m_RakPeer;
    }
};
}
