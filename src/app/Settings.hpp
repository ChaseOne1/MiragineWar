#pragma once
#include "app/ScriptModule.hpp"

namespace app {
class Settings final : public utility::Singleton<Settings>
{
    friend class Singleton<Settings>;
    friend class ScriptModule<Settings>;

private:
    static constexpr std::string_view msc_settings_file_name = "settings.lua";
    static constexpr std::string_view msc_language_CHS_file_name = "language_chs.lua";

    sol::table m_settings, m_language;

private:
    Settings();

    ~Settings() = default;

public:

    static sol::table& GetSettings() { return GetInstance().m_settings; }
    static sol::table& GetLanguage() { return GetInstance().m_language; }
};
}
