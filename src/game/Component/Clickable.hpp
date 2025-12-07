#pragma once

namespace game::comp {
struct Clickable
{
    std::function<void()> m_ClickInCallback {}, m_ClickOutCallback {};

    struct
    {
        float cx, cy, hw, hh;
    } m_Area;

    template <typename F>
    Clickable(F&& onClickInCallback, float cx, float cy, float hw, float hh)
        : m_ClickInCallback(std::forward<F>(onClickInCallback))
        , m_Area { cx, cy, hw, hh }
    { }

    template <typename F1, typename F2>
    Clickable(F1&& onClickInCallback, F2&& onClickOutCallback, float cx, float cy, float hw, float hh)
        : m_ClickInCallback(std::forward<F1>(onClickInCallback))
        , m_ClickOutCallback(std::forward<F2>(onClickOutCallback))
        , m_Area { cx, cy, hw, hh }
    { }
};
}
