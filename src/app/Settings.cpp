#include "Settings.hpp"
#include "app/ScriptManager.hpp"

using namespace app;

Settings::Settings()
    : m_system(ScriptManager::GetLuaState().script(R"(return require("configs.system"))"))
    , m_language(ScriptManager::GetLuaState().script(R"(return require("configs.language_chs"))"))
    , m_user(ScriptManager::GetLuaState().script(R"(return require("configs.user"))"))
{
    // NOTE: DO NOT do any action except return a value in these file
}
