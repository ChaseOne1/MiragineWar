#pragma once
#include "app/NetPacket.hpp"

namespace game::sys {

class NetGame : public utility::Singleton<NetGame>
{
    friend class utility::Singleton<NetGame>;

private:

public:
    NetGame();
    ~NetGame() = default;

    void OnMatchmakingRsp(const app::InboundPacket& packet);
    void OnMatchFinishedNotify(const app::InboundPacket& packet);
};
}
