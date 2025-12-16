#pragma once

namespace app {
template <typename T>
class ScriptModule
{
public:
    void RegisterToEnv(sol::environment& env)
    {
        static_cast<T*>(this)->RegisterEnv(env);
    }
};
}
