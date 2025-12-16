#pragma once
#include "app/ScriptModule.hpp"

namespace utility {
class Registry : public Singleton<Registry>, public app::ScriptModule<Registry>
{
    friend class Singleton<Registry>;
    friend class app::ScriptModule<Registry>;
private:
    entt::registry m_Registry;

private:
    Registry() = default;
    ~Registry() = default;

    void RegisterEnv(sol::environment& env);

public:
    static entt::registry& GetRegistry() noexcept { return GetInstance().m_Registry; }
};
}
