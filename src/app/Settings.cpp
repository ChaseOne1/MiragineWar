#include "Settings.hpp"
#include "app/ScriptManager.hpp"

using namespace app;

Settings::Settings()
    : m_settings(ScriptManager::GetLuaState().unsafe_script_file(ScriptManager::GetScriptFilePathString(msc_settings_file_name)))
    , m_language(ScriptManager::GetLuaState().unsafe_script_file(ScriptManager::GetScriptFilePathString(msc_language_CHS_file_name)))
{
    // NOTE: DO NOT do any action except return a value in these file
}
