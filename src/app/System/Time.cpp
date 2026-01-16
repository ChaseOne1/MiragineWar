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
    type["GameStartTimeSec"] = []() -> lua_Integer { return std::chrono::duration_cast<std::chrono::seconds>(Time::GameStartTime().time_since_epoch()).count(); };
    type["GameStartTimeMSec"] = []() -> lua_Integer { return std::chrono::duration_cast<std::chrono::milliseconds>(Time::GameStartTime().time_since_epoch()).count(); };
    type["NowSec"] = []() -> lua_Integer { return std::chrono::duration_cast<std::chrono::seconds>(Time::Now().time_since_epoch()).count(); };
    type["NowMSec"] = []() -> lua_Integer { return std::chrono::duration_cast<std::chrono::milliseconds>(Time::Now().time_since_epoch()).count(); };
    type["RealNowSec"] = []() -> lua_Integer { return std::chrono::duration_cast<std::chrono::seconds>(Time::RealNow().time_since_epoch()).count(); };
    type["RealNowMSec"] = []() -> lua_Integer { return std::chrono::duration_cast<std::chrono::milliseconds>(Time::RealNow().time_since_epoch()).count(); };
    type["DeltaTimeMSec"] = []() -> lua_Integer { return Time::DeltaTime().count(); };
    type["RealDeltaTimeMSec"] = []() -> lua_Integer { return Time::RealDeltaTime().count(); };
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
