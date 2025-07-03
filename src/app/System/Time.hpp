#pragma once

namespace app::sys {
class Time : public utility::Singleton<Time>
{
    friend class utility::Singleton<Time>;

private:
    Time() = default;
    ~Time() = default;

private:
    static constexpr float m_fFixedDeltaTime = 1.f / 60.f * 1000u;

    uint64_t m_uRealTime = 0u, m_uUnscaledDeltaTime = 0u;
    float m_fDeltaTime = 0.f, m_fTimeScale = 1.f;

public:
    void Tick();

    float GetDeltaTime() const noexcept { return m_fDeltaTime; }
    float GetDeltaTimeInSeconds() const noexcept { return m_fDeltaTime / 1000.f; }
    uint64_t GetUnscaledDeltaTime() const noexcept { return m_uUnscaledDeltaTime; }
    float GetUnscaledDeltaTimeInSeconds() const noexcept { return m_uUnscaledDeltaTime / 1000.f; }

    float GetFixedDeltaTime() const noexcept { return m_fFixedDeltaTime; }
    bool ConsumeTick() const noexcept;

    void SetTimeScale(float scale) noexcept { m_fTimeScale = scale; }
    float GetTimeScale() const noexcept { return m_fTimeScale; }

    uint64_t GetRealTime() const noexcept { return m_uRealTime; }
};
}
