#pragma once
#include "utility/Math/FRect.hpp"

namespace game::comp {
struct Collision
{
    using Callback_t = std::function<void()>;

    //center_x, center_y, half_w, half_h
    utility::math::FRect m_CollisionBox {};
    Callback_t m_fnOnCollide;
};
}
