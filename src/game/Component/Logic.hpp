#pragma once

namespace game::comp {
struct Logic
{
    using LogicFunc_t = std::function<void()>;

public:
    LogicFunc_t m_fnEnter {}, m_fnTick {}, m_fnExit {};

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
