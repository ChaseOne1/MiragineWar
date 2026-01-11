#pragma once
#include "app/ScriptComponent.hpp"

namespace game::comp {
struct Logic : app::ScriptComponent<Logic>
{
public:
    std::function<void()> m_fnEnter {}, m_fnTick {}, m_fnExit {};

public:
    template <typename T1, typename T2, typename T3>
    Logic(T1&& fnEnter, T2&& fnTick, T3&& fnExit)
        : m_fnEnter(std::forward<T1>(fnEnter))
        , m_fnTick(std::forward<T2>(fnTick))
        , m_fnExit(std::forward<T3>(fnExit))
    {
        if (m_fnEnter) m_fnEnter();
    }

    template <typename T>
    explicit Logic(T&& fnTick)
        : m_fnTick(std::forward<T>(fnTick))
    { }

    ~Logic()
    {
        if (m_fnExit) m_fnExit();
    }
};
}

#include "mirrow/srefl/srefl_begin.hpp"
// clang-format off
srefl_class(game::comp::Logic,
    ctors(
        ctor(std::function<void()>),
        ctor(std::function<void()>, std::function<void()>, std::function<void()>)
    )
)
// clang-format on
#include "mirrow/srefl/srefl_end.hpp"
