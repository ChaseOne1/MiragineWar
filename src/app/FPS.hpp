#pragma once
#include "game/Utility/Message.hpp"

namespace app{
class FPS : public utility::Singleton<FPS>
{
private:
    friend class utility::Singleton<FPS>;

private:
    FPS();
    ~FPS() = default;

private:
    // game::util::Message m_FPS;

public:

};
}

