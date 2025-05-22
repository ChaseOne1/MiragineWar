#pragma once
#include "utility/Math/Vector2.hpp"

namespace game::comp {
struct Movement
{
    utility::math::Vector2 m_Velocity {};
    utility::math::Vector2 m_Acceleration {};
};
}
