#pragma once
#include "app/ScriptComponent.hpp"

namespace game::comp {
struct UIClickable : app::ScriptComponent<UIClickable>
{
    using Callback_t = std::function<void()>;
    Callback_t m_ClickInCallback {}, m_ClickOutCallback {};

    mathfu::vec4 m_Area;

    template <typename F>
    UIClickable(F&& onClickInCallback, float cx, float cy, float hw, float hh)
        : m_ClickInCallback(std::forward<F>(onClickInCallback))
        , m_Area { cx, cy, hw, hh }
    { }

    template <typename F1, typename F2>
    UIClickable(F1&& onClickInCallback, F2&& onClickOutCallback, float cx, float cy, float hw, float hh)
        : m_ClickInCallback(std::forward<F1>(onClickInCallback))
        , m_ClickOutCallback(std::forward<F2>(onClickOutCallback))
        , m_Area { cx, cy, hw, hh }
    { }
};
}

#include "mirrow/srefl/srefl_begin.hpp"
// clang-format off
srefl_class(game::comp::UIClickable,
    ctors(
        ctor(sol::function, float, float, float, float),
        ctor(sol::function, sol::function, float, float, float, float)
    )
)
// clang-format on
#include "mirrow/srefl/srefl_end.hpp"
