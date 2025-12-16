#pragma once
#include "slikenet/peerinterface.h"
#include "slikenet/MessageIdentifiers.h"
#include "slikenet/types.h"
#include "utility/Topics.hpp"
#include "app/NetPacket.hpp"
#include "app/ScriptModule.hpp"

namespace app::sys {
class Network : public utility::Singleton<Network>, public app::ScriptModule<Network>
{
    friend class utility::Singleton<Network>;

    using Handler = std::function<void(const InboundPacket&)>;

private:
    SLNet::RakPeerInterface* m_RakPeer;

    utility::Topics<SLNet::MessageID, Handler> m_MessageHandler;

private:
    void RegisterEnv(sol::environment&);

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

    static auto GetRakPeerInterface()
    {
        return GetInstance().m_RakPeer;
    }
};
}
