#pragma once

namespace app {
class Settings final : public utility::Singleton<Settings>
{
    friend class Singleton<Settings>;

private:
    static constexpr std::string_view msc_user_settings_name = "user";
    static constexpr std::string_view msc_system_settings_name = "system";
    static constexpr std::string_view msc_language_file_name = "language_chs";

    sol::table m_user, m_system, m_language;

private:
    Settings();

    ~Settings() = default;

public:
    static sol::table& GetUser() { return GetInstance().m_user; }
    static sol::table& GetSystem() { return GetInstance().m_system; }
    static sol::table& GetLanguage() { return GetInstance().m_language; }
};
}
