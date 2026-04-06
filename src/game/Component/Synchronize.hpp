#pragma once

namespace game::comp {
struct Synchronize
{
    entt::id_type m_RemoteID;

    mathfu::vec2 m_Position = mathfu::kZeros2f, m_Velocity = mathfu::kZeros2f;

    Synchronize(entt::id_type remote_id)
        : m_RemoteID(remote_id)
    { }
};
}

#include "mirrow/srefl/srefl_begin.hpp"
// clang-format off
srefl_class(game::comp::Synchronize,
    ctors(
        ctor(entt::id_type)
    )

    fields(
        field(&game::comp::Synchronize::m_RemoteID)
        )
)
// clang-format on
#include "mirrow/srefl/srefl_end.hpp"
