#pragma once

namespace app::sys {
class Time : public utility::Singleton<Time>
{
    friend class utility::Singleton<Time>;

public:
    using Clock_t = std::chrono::steady_clock;

    static constexpr std::chrono::milliseconds msc_FixedDeltaTime { 16 };

private:
    Time();
    ~Time() = default;

private:
    const std::chrono::time_point<Clock_t> mc_GameStartTime;

    std::chrono::time_point<Clock_t> m_Now, m_RealNow;
    std::chrono::milliseconds m_DeltaTime, m_RealDeltaTime;

    float m_fTimeScale = 1.f;

public:
    void Tick();

    static bool FixedTick() noexcept;

    static std::chrono::time_point<std::chrono::steady_clock> GameStartTime() noexcept { return GetInstance().mc_GameStartTime; }
    static uint64_t GameStartTimeSec() noexcept { return std::chrono::duration_cast<std::chrono::seconds>(GameStartTime().time_since_epoch()).count(); }
    static uint64_t GameStartTimeMSec() noexcept { return std::chrono::duration_cast<std::chrono::milliseconds>(GameStartTime().time_since_epoch()).count(); }

    static std::chrono::time_point<std::chrono::steady_clock> Now() noexcept { return GetInstance().m_Now; }
    static uint64_t NowSec() noexcept { return std::chrono::duration_cast<std::chrono::seconds>(Now().time_since_epoch()).count(); }
    static uint64_t NowMSec() noexcept { return std::chrono::duration_cast<std::chrono::milliseconds>(Now().time_since_epoch()).count(); }

    static std::chrono::time_point<std::chrono::steady_clock> RealNow() noexcept { return GetInstance().m_RealNow; }
    static uint64_t RealNowSec() noexcept { return std::chrono::duration_cast<std::chrono::seconds>(RealNow().time_since_epoch()).count(); }
    static uint64_t RealNowMSec() noexcept { return std::chrono::duration_cast<std::chrono::milliseconds>(RealNow().time_since_epoch()).count(); }

    static std::chrono::milliseconds FixedDeltaTime() noexcept { return msc_FixedDeltaTime; }
    static double FixedDeltaTimeSec() noexcept { return std::chrono::duration_cast<std::chrono::duration<double>>(FixedDeltaTime()).count(); }
    static uint64_t FixedDeltaTimeMSec() noexcept { return FixedDeltaTime().count(); }

    static std::chrono::milliseconds DeltaTime() noexcept { return GetInstance().m_DeltaTime; }
    static uint64_t DeltaTimeMSec() noexcept { return DeltaTime().count(); }
    static double DeltaTimeSec() noexcept { return std::chrono::duration_cast<std::chrono::duration<double>>(DeltaTime()).count(); }

    static std::chrono::milliseconds RealDeltaTime() noexcept { return GetInstance().m_RealDeltaTime; }
    static uint64_t RealDeltaTimeMSec() noexcept { return RealDeltaTime().count(); }
};
}
