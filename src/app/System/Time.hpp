#pragma once

namespace app::sys {
class Time : public utility::Singleton<Time>
{
    friend class utility::Singleton<Time>;

private:
    Time() = default;
    ~Time()= default;

private:
    float m_DeltaTime = 0u, m_UnscaledDeltaTime = 0u, m_FixedDeltaTime = 1.f / 60.f;

    float m_TimeScale = 1.f;

    uint64_t m_RealTime = 0u;

public:
    void Tick();

    float GetDeltaTime() const noexcept { return m_DeltaTime; }
    float GetUnscaledDeltaTime() const noexcept { return m_UnscaledDeltaTime; }

    float GetFixedDeltaTime() const noexcept;
    bool ConsumeFixedTick() const noexcept;

    void SetTimeScale(float scale) noexcept { m_TimeScale = scale; }
    float GetTimeScale() const noexcept { return m_TimeScale; }

    uint64_t GetRealTime() const noexcept { return m_RealTime; }
};
}
