#include "Settings.hpp"
#include "app/ScriptManager.hpp"

using namespace app;

Settings::Settings()
    : m_settings(ScriptManager::GetLuaState().script(R"(return require("settings"))"))
    , m_language(ScriptManager::GetLuaState().script(R"(return require("language_chs"))"))
{
    // NOTE: DO NOT do any action except return a value in these file
}
