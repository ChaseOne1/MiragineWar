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

    const std::chrono::time_point<std::chrono::steady_clock>& Now() const noexcept { return m_Now; }
    const std::chrono::time_point<std::chrono::steady_clock>& RealNow() const noexcept { return m_RealNow; }
    
    const std::chrono::milliseconds DeltaTime() const noexcept {return m_DeltaTime;}
    const std::chrono::milliseconds RealDeltaTime() const noexcept {return m_RealDeltaTime;}
};
}
