#pragma once
#include "ScriptModule.hpp"

namespace app {
class ScriptLib : public utility::Singleton<ScriptLib>, public app::ScriptModule<ScriptLib>
{
    friend class Singleton;
    friend class ScriptModule;

private:
    ScriptLib() = default;
    ~ScriptLib() = default;

    void RegisterEnv(sol::environment&);

public:

};
}