#pragma once

namespace app::sys {
class Time : public utility::Singleton<Time>
{
    friend class utility::Singleton<Time>;

private:
    Time();
    ~Time() = default;

private:
    static constexpr std::chrono::milliseconds m_FixedDeltaTime { 16 };
    const std::chrono::time_point<std::chrono::steady_clock> mc_GameStartTime;

    std::chrono::time_point<std::chrono::steady_clock> m_Now, m_RealNow;
    std::chrono::milliseconds m_DeltaTime, m_RealDeltaTime;

    float m_fTimeScale = 1.f;

public:
    void Tick();

    bool FixedTick() const noexcept;

    static const std::chrono::time_point<std::chrono::steady_clock>& GameStartTime() { return GetInstance().mc_GameStartTime; }

    static const std::chrono::time_point<std::chrono::steady_clock>& Now() noexcept { return GetInstance().m_Now; }
    static const std::chrono::time_point<std::chrono::steady_clock>& RealNow() noexcept { return GetInstance().m_RealNow; }

    static const std::chrono::milliseconds DeltaTime() noexcept { return GetInstance().m_DeltaTime; }
    static const std::chrono::milliseconds RealDeltaTime() noexcept { return GetInstance().m_RealDeltaTime; }
};
}
