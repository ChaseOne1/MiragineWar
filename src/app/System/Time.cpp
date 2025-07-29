#include "Time.hpp"

using namespace app::sys;
using namespace std::chrono;

static milliseconds gs_FixedTimeAccumulator { 0 };

Time::Time()
    : mc_GameStartTime(std::chrono::steady_clock::now())
{ }

void Time::Tick()
{
    static time_point<std::chrono::steady_clock> real_last = mc_GameStartTime;
    const time_point<std::chrono::steady_clock> now = steady_clock::now();

    m_RealDeltaTime = duration_cast<milliseconds>(now - real_last);
    m_RealNow += m_RealDeltaTime;

    m_DeltaTime = duration_cast<milliseconds>(m_RealDeltaTime * m_fTimeScale);
    m_Now += m_DeltaTime;

    gs_FixedTimeAccumulator += m_DeltaTime;

    real_last = now;
}

bool Time::FixedTick() const noexcept
{
    if (gs_FixedTimeAccumulator < m_FixedDeltaTime) return false;

    gs_FixedTimeAccumulator -= m_FixedDeltaTime;
    return true;
}
