#pragma once

namespace game::comp {
struct Movement
{
    mathfu::vec2 m_Velocity { mathfu::kZeros2f };
    mathfu::vec2 m_Acceleration { mathfu::kZeros2f };
};
}
