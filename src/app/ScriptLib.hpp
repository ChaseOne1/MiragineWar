#pragma once

namespace app {
class ScriptLib : public utility::Singleton<ScriptLib>
{
    friend class Singleton;

private:
    ScriptLib();
    ~ScriptLib() = default;
};
}
