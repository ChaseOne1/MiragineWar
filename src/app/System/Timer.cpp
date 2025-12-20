#include "Timer.hpp"

using namespace app::sys;

void Timer::Tick()
{
    const auto now = app::sys::Time::Now();
    while (!m_TimerHeap.empty()) {
        if (const std::shared_ptr timer = m_TimerHeap.top().lock(); !timer) m_TimerHeap.pop();
        else if (timer->m_NextCallTime <= now) {
            if (timer->m_Callable()) {
                timer->m_NextCallTime += timer->m_Interval;
                m_TimerHeap.pop();
                m_TimerHeap.emplace(timer);
            } else {
                m_TimerHeap.pop();
                // TODO: store the invalid timers and clean outside the loop
                m_TimerMap.erase(timer->m_TimerId);
            }
        } else break;
    }
}