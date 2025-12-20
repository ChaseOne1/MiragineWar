#pragma once
#include "app/System/Time.hpp"

namespace app::sys {
class Timer : public utility::Singleton<Timer>
{
public:
    using TimerId = std::uint32_t;
    static constexpr TimerId TimerIdNull = 0;

private:
    friend class utility::Singleton<Timer>;

    struct TimerInfo
    {
        const TimerId m_TimerId;
        const std::chrono::milliseconds m_Interval;
        std::chrono::time_point<std::chrono::steady_clock> m_NextCallTime;
        std::function<bool()> m_Callable;

    public:
        TimerInfo(TimerId timerId, std::chrono::milliseconds interval, std::function<bool()> callable)
            : m_TimerId(timerId)
              , m_Interval(interval)
              , m_NextCallTime(Time::Now() + interval)
              , m_Callable(std::move(callable)) {}

        bool operator>(const TimerInfo& other) const { return m_NextCallTime > other.m_NextCallTime; }

    public:
        struct WeakPtrGreaterWrapper
        {
            bool operator()(const std::weak_ptr<TimerInfo>& lhs, const std::weak_ptr<TimerInfo>& rhs) const { return lhs.expired() || rhs.expired() || lhs.lock()->m_NextCallTime > rhs.lock()->m_NextCallTime; }
        };
    };

private:
    TimerId m_NextTimerId = 1u;
    std::priority_queue<std::weak_ptr<TimerInfo>, std::vector<std::weak_ptr<TimerInfo>>, TimerInfo::WeakPtrGreaterWrapper> m_TimerHeap;
    std::unordered_map<TimerId, std::shared_ptr<TimerInfo>> m_TimerMap;

private:
    Timer() = default;
    ~Timer() = default;

public:
    void Tick();

    template <typename F, typename = std::enable_if<std::is_invocable_r_v<bool, F>>>
    static TimerId AddTimer(std::chrono::milliseconds interval, F&& callable, bool callNow = false)
    {
        auto& timer = GetInstance();
        const auto [piter, succ] = timer.m_TimerMap.emplace(timer.m_NextTimerId, std::make_shared<TimerInfo>(timer.m_NextTimerId, interval, std::forward<F>(callable)));
        if (!succ) return TimerIdNull;

        timer.m_TimerHeap.emplace(piter->second);

        if (callNow) piter->second->m_Callable();

        return timer.m_NextTimerId++;
    }

    static void DelTimer(TimerId timerId) { GetInstance().m_TimerMap.erase(timerId); }
};
}