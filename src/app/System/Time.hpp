#pragma once
#include "app/ScriptModule.hpp"

namespace app::sys {
class Time : public utility::Singleton<Time>, public app::ScriptModule<Time>
{
    friend class utility::Singleton<Time>;
    friend class ScriptModule;

public:
    using Clock_t = std::chrono::steady_clock;

    static constexpr std::chrono::milliseconds m_FixedDeltaTime { 16 };

private:
    Time();
    ~Time() = default;

private:
    const std::chrono::time_point<Clock_t> mc_GameStartTime;

    std::chrono::time_point<Clock_t> m_Now, m_RealNow;
    std::chrono::milliseconds m_DeltaTime, m_RealDeltaTime;

    float m_fTimeScale = 1.f;

    void RegisterEnv(sol::environment& env);

public:
    void Tick();

    static bool FixedTick() noexcept;

    static std::chrono::time_point<std::chrono::steady_clock> GameStartTime() { return GetInstance().mc_GameStartTime; }
    static uint64_t GameStartTimeSec() { return std::chrono::duration_cast<std::chrono::seconds>(GameStartTime().time_since_epoch()).count(); }
    static uint64_t GameStartTimeMSec() { return std::chrono::duration_cast<std::chrono::milliseconds>(GameStartTime().time_since_epoch()).count(); }

    static std::chrono::time_point<std::chrono::steady_clock> Now() noexcept { return GetInstance().m_Now; }
    static uint64_t NowSec() { return std::chrono::duration_cast<std::chrono::seconds>(Now().time_since_epoch()).count(); }
    static uint64_t NowMSec() { return std::chrono::duration_cast<std::chrono::milliseconds>(Now().time_since_epoch()).count(); }

    static std::chrono::time_point<std::chrono::steady_clock> RealNow() noexcept { return GetInstance().m_RealNow; }
    static uint64_t RealNowSec() { return std::chrono::duration_cast<std::chrono::seconds>(RealNow().time_since_epoch()).count(); }
    static uint64_t RealNowMSec() { return std::chrono::duration_cast<std::chrono::milliseconds>(RealNow().time_since_epoch()).count(); }

    static std::chrono::milliseconds DeltaTime() noexcept { return GetInstance().m_DeltaTime; }
    static uint64_t DeltaTimeMSec() { return DeltaTime().count(); }

    static std::chrono::milliseconds RealDeltaTime() noexcept { return GetInstance().m_RealDeltaTime; }
    static uint64_t RealDeltaTimeMSec() { return RealDeltaTime().count(); }
};
}
