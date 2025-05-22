#pragma once
#include <toml++/toml.hpp>

#define APP_SETTINGS_FILE "settings.toml"

namespace app {
class Settings final : public utility::Singleton<Settings> {
    friend class Singleton<Settings>;

private:
    toml::table m_settings;

private:
    Settings()
        : m_settings(toml::parse_file(APP_SETTINGS_FILE))
    {
    }

    ~Settings() = default;

public:
    toml::table& GetSettings() { return m_settings; }
};
}

#undef APP_SETTINGS_FILE
