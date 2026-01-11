#pragma once
#include "app/ScriptComponent.hpp"

namespace game::comp {
struct UIPointable : app::ScriptComponent<UIPointable>
{
    using Callback_t = std::function<void()>;
    Callback_t m_PointInCallback {}, m_PointOutCallback {};

    mathfu::vec4 m_Area;

    template <typename F>
    explicit UIPointable(F&& onPointInCallback, float cx, float cy, float hw, float hh)
        : m_PointInCallback(std::forward<F>(onPointInCallback))
        , m_Area { cx, cy, hw, hh }
    { }

    template <typename F1, typename F2>
    explicit UIPointable(F1&& onPointInCallback, F2&& onPointOutCallback, float cx, float cy, float hw, float hh)
        : m_PointInCallback(std::forward<F1>(onPointInCallback))
        , m_PointOutCallback(std::forward<F2>(onPointOutCallback))
        , m_Area { cx, cy, hw, hh }
    { }

};
}

#include "mirrow/srefl/srefl_begin.hpp"
// clang-format off
srefl_class(game::comp::UIPointable,
    ctors(
        ctor(sol::function, float, float, float, float),
        ctor(sol::function, sol::function, float, float, float, float)
    )
)
// clang-format on
#include "mirrow/srefl/srefl_end.hpp"
