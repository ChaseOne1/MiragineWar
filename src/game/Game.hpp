#pragma once
#include "utility/Registry.hpp"

namespace game {
class Game : public utility::Singleton<Game>
{
    friend class utility::Singleton<Game>;

private:
    entt::entity m_Game = utility::Registry::GetInstance().GetRegistry().create();

private:
    Game();
    ~Game() = default;

    void LogicInit();
};
}
