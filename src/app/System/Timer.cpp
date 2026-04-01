#include "Timer.hpp"
#include "utility/ScriptManager.hpp"

using namespace app::sys;

Timer::Timer()
{
    sol::state_view state = utility::ScriptManager::GetLuaState();
    sol::table timer = state.script(R"(return require("builtin.timer"))");

    timer["add_timer"] = [](lua_Integer ms, sol::function f, bool callNow)
        -> lua_Integer { return GetInstance().AddTimer(std::chrono::milliseconds(ms), f, callNow); };
    timer["del_timer"] = DelTimer;
}

void Timer::Tick()
{
    std::vector<TimerInfo*> rescheduled;

    const auto now = app::sys::Time::Now();
    while (!m_TimerHeap.empty()) {
        if (TimerInfo* timer = m_TimerHeap.top(); !timer->m_Active) {
            m_TimerHeap.pop();
            m_TimerMap.erase(timer->m_TimerId);
        } else if (timer->m_NextCallTime <= now) {
            m_TimerHeap.pop();
            if (timer->m_Callable()) {
                timer->m_NextCallTime += timer->m_Interval;
                rescheduled.push_back(timer);
            } else {
                // TODO: store the invalid timers and clean outside the loop
                m_TimerMap.erase(timer->m_TimerId);
            }
        } else break;
    }

    for (auto timer : rescheduled) {
        m_TimerHeap.emplace(timer);
    }
}

void Timer::DelTimer(TimerId timerId)
{
    if (auto iter = GetInstance().m_TimerMap.find(timerId); iter != GetInstance().m_TimerMap.end()) {
        iter->second.m_Active = false;
    }
}
