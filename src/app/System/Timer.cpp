#include "Timer.hpp"

using namespace app::sys;

void Timer::Tick()
{
    std::vector<TimerInfo*> rescheduled;

    const auto now = app::sys::Time::Now();
    while (!m_TimerHeap.empty()) {
        if (TimerInfo* timer = m_TimerHeap.top(); !timer->m_Active) {
            m_TimerHeap.pop();
            m_TimerMap.erase(timer->m_TimerId);
        } else if (timer->m_NextCallTime <= now) {
            if (timer->m_Callable()) {
                timer->m_NextCallTime += timer->m_Interval;
                rescheduled.push_back(timer);
                m_TimerHeap.pop();
            } else {
                m_TimerHeap.pop();
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
