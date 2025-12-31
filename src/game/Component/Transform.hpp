#pragma once

namespace game::comp {
struct Transform
{
    mathfu::vec2 m_Position { mathfu::kZeros2f };
    mathfu::vec2 m_HalfSize { mathfu::kZeros2f };

public:
    Transform() = default;

    Transform(float x, float y)
        : m_Position(x, y)
    { }

    Transform(float x, float y, float hw, float hh)
        : m_Position(x, y)
        , m_HalfSize(hw, hh)
    { }

    Transform(const mathfu::vec2& pos, const mathfu::vec2& halfSize) : m_Position(pos), m_HalfSize(halfSize)
    { }

    std::array<mathfu::vec3, 4> GetVertices() const noexcept
    {
        return {
            mathfu::vec3 { m_Position.x - m_HalfSize.x, m_Position.y - m_HalfSize.y, 1.f },
            mathfu::vec3 { m_Position.x + m_HalfSize.x, m_Position.y - m_HalfSize.y, 1.f },
            mathfu::vec3 { m_Position.x - m_HalfSize.x, m_Position.y + m_HalfSize.y, 1.f },
            mathfu::vec3 { m_Position.x + m_HalfSize.x, m_Position.y + m_HalfSize.y, 1.f },
        };
    }

    SDL_FRect GetFRect() const noexcept
    {
        return {
            m_Position.x - m_HalfSize.x, m_Position.y - m_HalfSize.y,
            m_HalfSize.x * 2.f, m_HalfSize.y * 2.f
        };
    }
};
}

#include "mirrow/srefl/srefl_begin.hpp"
srefl_class(game::comp::Transform,
    ctors(
        ctor(float, float, float, float)
    )
    fields(
        field(&game::comp::Transform::m_Position),
        field(&game::comp::Transform::m_HalfSize)
    )
)
#include "mirrow/srefl/srefl_end.hpp"
