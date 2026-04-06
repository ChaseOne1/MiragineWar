#include "Time.hpp"
#include "utility/ScriptManager.hpp"

using namespace app::sys;
using namespace std::chrono;

static milliseconds gs_FixedTimeAccumulator { 0 };

Time::Time()
    : mc_GameStartTime(Clock_t::now())
    , m_DeltaTime()
    , m_RealDeltaTime()
{
    sol::state_view state = utility::ScriptManager::GetLuaState();
    sol::table prototype = state.script(R"(return require("builtin.time"))");

    prototype["game_start_time_sec"] = []() -> lua_Integer { return GameStartTimeSec(); };
    prototype["game_start_time_msec"] = []() -> lua_Integer { return GameStartTimeMSec(); };
    prototype["now_sec"] = []() -> lua_Integer { return NowSec(); };
    prototype["now_msec"] = []() -> lua_Integer { return NowMSec(); };
    prototype["real_now_sec"] = []() -> lua_Integer { return RealNowSec(); };
    prototype["real_now_msec"] = []() -> lua_Integer { return RealNowMSec(); };
    prototype["delta_time_msec"] = []() -> lua_Integer { return DeltaTimeMSec(); };
    prototype["real_delta_time_msec"] = []() -> lua_Integer { return RealDeltaTimeMSec(); };
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
    if (gs_FixedTimeAccumulator < msc_FixedDeltaTime) return false;

    gs_FixedTimeAccumulator -= msc_FixedDeltaTime;
    return true;
}
