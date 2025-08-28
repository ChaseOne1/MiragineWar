#pragma once

namespace game::comp {
struct Pointable
{
    std::function<void()> m_PointInCallback {}, m_PointOutCallback {};

    struct
    {
        float cx, cy, hw, hh;
    } m_Area;

    template <typename F>
    explicit Pointable(F&& onPointInCallback, float cx, float cy, float hw, float hh)
        : m_PointInCallback(std::forward<F>(onPointInCallback))
        , m_Area { cx, cy, hw, hh }
    { }

    template <typename F1, typename F2>
    explicit Pointable(F1&& onPointInCallback, F2&& onPointOutCallback, float cx, float cy, float hw, float hh)
        : m_PointInCallback(std::forward<F1>(onPointInCallback))
        , m_PointOutCallback(std::forward<F2>(onPointOutCallback))
        , m_Area { cx, cy, hw, hh }
    { }

};
}
