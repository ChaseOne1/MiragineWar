#pragma once
#include "utility/Math/FRect.hpp"

namespace game::comp
{
    struct Collision
    {
        using Callback_t = std::function<void()>;

        utility::math::FRect m_CollisionBox;
        Callback_t m_fnOnCollide;

    };

    struct FixedCollision
    {
        using Callback_t = std::function<void()>;

        Callback_t m_fnOnCollide;
    };
}
