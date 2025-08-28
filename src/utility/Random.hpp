#pragma once
#include <random>

namespace utility {
class Random : public Singleton<Random>
{
    friend class Singleton<Random>;

private:
    std::mt19937 m_Generator { std::random_device {}() };

private:
    Random() =default;
    ~Random() = default;

public:
    static std::mt19937& GetGenerator() noexcept { return GetInstance().m_Generator; }
};
}
