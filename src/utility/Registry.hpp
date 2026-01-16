#pragma once

#include <iostream>
namespace utility {
class Registry : public Singleton<Registry>
{
    friend class Singleton<Registry>;

private:
    entt::registry m_Registry;

private:
    Registry() = default;
    ~Registry() = default;

public:
    static entt::registry& GetRegistry() noexcept { return GetInstance().m_Registry; }
    static void RegisterToLua();
};
}
