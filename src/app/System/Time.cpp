#include "Time.hpp"

using namespace app::sys;

static float gs_FixedTimeAccumulator = 0.f;

void Time::Tick()
{
    static uint64_t last = 0u;

    m_RealTime = SDL_GetTicks();

    m_UnscaledDeltaTime = (m_RealTime - last) / 1000.f;
    m_DeltaTime = m_UnscaledDeltaTime * m_TimeScale;

    gs_FixedTimeAccumulator += m_DeltaTime;

    last = m_RealTime;
}

bool Time::ConsumeFixedTick() const noexcept
{
    if (gs_FixedTimeAccumulator < m_FixedDeltaTime) return false;

    gs_FixedTimeAccumulator -= m_FixedDeltaTime;
    return true;
}
