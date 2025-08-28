#pragma once

namespace game::sys {
class NetGame : public utility::Singleton<NetGame>
{
    friend class utility::Singleton<NetGame>;

public:
    NetGame();
    ~NetGame() = default;
};
}
