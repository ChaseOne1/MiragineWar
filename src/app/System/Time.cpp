#include "Time.hpp"

using namespace app::sys;

static uint64_t gs_GameStartTime = 0u;
static float gs_fFixedTimeAccumulator = 0.f;

Time::Time()
{
    gs_GameStartTime = SDL_GetTicks();
}

void Time::Tick()
{
    static uint64_t last = 0u;

    m_uRealTime = SDL_GetTicks() - gs_GameStartTime;

    m_uUnscaledDeltaTime = m_uRealTime - last;
    m_fDeltaTime = m_uUnscaledDeltaTime * m_fTimeScale;

    gs_fFixedTimeAccumulator += m_fDeltaTime;

    last = m_uRealTime;
}

bool Time::ConsumeTick() const noexcept
{
    if (gs_fFixedTimeAccumulator < m_fFixedDeltaTime) return false;

    gs_fFixedTimeAccumulator -= m_fFixedDeltaTime;
    return true;
}
