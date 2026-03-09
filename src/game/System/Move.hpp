#pragma once
#include "app/System/Network.hpp"

namespace game::sys {
class Move : public utility::Singleton<Move>
{
    friend class utility::Singleton<Move>;

private:
    Move();
    ~Move() = default;

    void OnPositionSync(const app::InboundPacket&);

public:
    void Tick();
};
}
