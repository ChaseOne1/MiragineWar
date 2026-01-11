#pragma once
#include "app/ScriptComponent.hpp"

namespace game::comp {
struct Movement : app::ScriptComponent<Movement>
{
    mathfu::vec2 m_Velocity { mathfu::kZeros2f };
    mathfu::vec2 m_Acceleration { mathfu::kZeros2f };

    explicit Movement(mathfu::vec2 velocity)
        : m_Velocity(velocity)
    { }

    Movement(mathfu::vec2 velocity, mathfu::vec2 acceleration)
        : m_Velocity(velocity)
        , m_Acceleration(acceleration)
    { }

    explicit Movement(sol::stack_table velocity)
        : m_Velocity(velocity.get<float>("x"), velocity.get<float>("y"))
    { }

    Movement(sol::stack_table velocity, sol::stack_table acceleration)
        : m_Velocity(velocity.get<float>("x"), velocity.get<float>("y"))
        , m_Acceleration(acceleration.get<float>("x"), acceleration.get<float>("y"))
    { }
};
}

#include "mirrow/srefl/srefl_begin.hpp"
// clang-format off
srefl_class(game::comp::Movement,
    ctors(
        ctor(sol::stack_table),
        ctor(sol::stack_table, sol::stack_table)
    )
    fields(
        field(&game::comp::Movement::m_Velocity),
        field(&game::comp::Movement::m_Acceleration)
    )
)
// clang-format on
#include "mirrow/srefl/srefl_end.hpp"
