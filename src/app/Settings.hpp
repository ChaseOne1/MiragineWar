#pragma once

#define APP_SETTINGS_FILE "settings.toml"
#define LANGUAGE_CN_SETTINGS_FILE "language_cn.toml"

namespace app {
class Settings final : public utility::Singleton<Settings>
{
    friend class Singleton<Settings>;

private:
    toml::table m_settings, m_language;

private:
    Settings()
        : m_settings(toml::parse_file(APP_SETTINGS_FILE))
        , m_language(toml::parse_file(LANGUAGE_CN_SETTINGS_FILE))
    { }

    ~Settings() = default;

public:
    static toml::table& GetSettings() { return GetInstance().m_settings; }
    static toml::table& GetLanguageSettings() { return GetInstance().m_language; }
};
}

#undef APP_SETTINGS_FILE
#undef LANGUAGE_CN_SETTINGS_FILE
