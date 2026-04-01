#include "Settings.hpp"
#include "utility/ScriptManager.hpp"

using namespace app;

Settings::Settings()
    : m_system(utility::ScriptManager::GetLuaState().script(R"(return require("configs.system"))"))
    , m_language(utility::ScriptManager::GetLuaState().script(R"(return require("configs.language_chs"))"))
    , m_user(utility::ScriptManager::GetLuaState().script(R"(return require("configs.user"))"))
{
}
