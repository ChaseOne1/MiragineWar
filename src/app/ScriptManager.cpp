#include "ScriptManager.hpp"
#include "app/Settings.hpp"

using namespace app;

ScriptManager::ScriptManager()
{
     m_LuaState.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::string, sol::lib::table);
}
