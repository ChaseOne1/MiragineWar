#pragma once
#include "app/System/Time.hpp"

namespace app::sys {
class Timer : public utility::Singleton<Timer>, public app::ScriptModule<Timer>
{
public:
    using TimerId = std::uint32_t;
    static constexpr TimerId TimerIdNull = 0;

private:
    friend class utility::Singleton<Timer>;
    friend class app::ScriptModule<Timer>;

    struct TimerInfo
    {
        std::chrono::time_point<std::chrono::steady_clock> m_NextCallTime;
        const std::chrono::milliseconds m_Interval;
        const TimerId m_TimerId;
        bool m_Active = true;
        std::function<bool()> m_Callable;

    public:
        TimerInfo(TimerId timerId, std::chrono::milliseconds interval, std::function<bool()> callable)
            : m_NextCallTime(Time::Now() + interval)
            , m_Interval(interval)
            , m_TimerId(timerId)
            , m_Callable(std::move(callable))
        { }
    };

    struct TimerInfoPtrCompare
    {
        bool operator()(TimerInfo* lhs, TimerInfo* rhs) const
        {
            return lhs->m_NextCallTime > rhs->m_NextCallTime;
        }
    };

private:
    TimerId m_NextTimerId = 1u;
    std::priority_queue<TimerInfo*, std::vector<TimerInfo*>, TimerInfoPtrCompare> m_TimerHeap;
    std::unordered_map<TimerId, TimerInfo> m_TimerMap;

private:
    Timer() = default;
    ~Timer() = default;

    void RegisterEnv(sol::environment&);

public:
    void Tick();

    template <typename F, typename = std::enable_if<std::is_invocable_r_v<bool, F>>>
    static TimerId AddTimer(std::chrono::milliseconds interval, F&& callable, bool callNow = false)
    {
        auto& timer = GetInstance();
        const auto [piter, succ] = timer.m_TimerMap.emplace(timer.m_NextTimerId, TimerInfo{ timer.m_NextTimerId, interval, std::forward<F>(callable)});
        if (!succ) return TimerIdNull;

        timer.m_TimerHeap.emplace(&piter->second);

        if (callNow) piter->second.m_Callable();

        return timer.m_NextTimerId++;
    }

    static void DelTimer(TimerId timerId);
};
}
