#pragma once

namespace game::comp {
struct Transform
{
    mathfu::vec2 m_Position { mathfu::kZeros2f };
    mathfu::vec2 m_HalfSize { mathfu::kZeros2f };

public:
    std::array<mathfu::vec3, 4> GetVertices() const noexcept
    {
        return {
            mathfu::vec3 { m_Position.x - m_HalfSize.x, m_Position.y - m_HalfSize.y, 1.f },
            mathfu::vec3 { m_Position.x + m_HalfSize.x, m_Position.y - m_HalfSize.y, 1.f },
            mathfu::vec3 { m_Position.x - m_HalfSize.x, m_Position.y + m_HalfSize.y, 1.f },
            mathfu::vec3 { m_Position.x + m_HalfSize.x, m_Position.y + m_HalfSize.y, 1.f },
        };
    }
};
}
