#include "Time.hpp"
#include "app/ScriptManager.hpp"

using namespace app::sys;
using namespace std::chrono;

static milliseconds gs_FixedTimeAccumulator { 0 };

Time::Time()
    : mc_GameStartTime(Clock_t::now())
    , m_DeltaTime()
    , m_RealDeltaTime()
{ }

void Time::RegisterToLua()
{
    auto type = app::ScriptManager::GetLuaState().new_usertype<Time>("Time", sol::no_constructor);
    type["GameStartTimeSec"] = []() -> lua_Integer { return GameStartTimeSec(); };
    type["GameStartTimeMSec"] = []() -> lua_Integer { return GameStartTimeMSec(); };
    type["NowSec"] = []() -> lua_Integer { return NowSec(); };
    type["NowMSec"] = []() -> lua_Integer { return NowMSec(); };
    type["RealNowSec"] = []() -> lua_Integer { return RealNowSec(); };
    type["RealNowMSec"] = []() -> lua_Integer { return RealNowMSec(); };
    type["DeltaTimeMSec"] = []() -> lua_Integer { return DeltaTimeMSec(); };
    type["RealDeltaTimeMSec"] = []() -> lua_Integer { return RealDeltaTimeMSec(); };
}

void Time::Tick()
{
    static time_point<Clock_t> real_last = mc_GameStartTime;
    const time_point<Clock_t> now = steady_clock::now();

    m_RealDeltaTime = duration_cast<milliseconds>(now - real_last);
    m_RealNow += m_RealDeltaTime;

    m_DeltaTime = duration_cast<milliseconds>(m_RealDeltaTime * m_fTimeScale);
    m_Now += m_DeltaTime;

    gs_FixedTimeAccumulator += m_DeltaTime;

    real_last = now;
}

bool Time::FixedTick() noexcept
{
    if (gs_FixedTimeAccumulator < m_FixedDeltaTime) return false;

    gs_FixedTimeAccumulator -= m_FixedDeltaTime;
    return true;
}
