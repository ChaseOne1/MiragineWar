#pragma once

#define APP_NAME "MiragineWar"
#define APP_VERSION "0.1.0"

namespace app {
struct AppMetaData final {
    AppMetaData() = delete;
    ~AppMetaData() = delete;

public:
    inline static constexpr char NAME[] = APP_NAME;
    inline static constexpr char VERSION[] = APP_VERSION;
    inline static const std::filesystem::path msc_szAssetsPath { "Assets/" };
};
}

#undef APP_NAME
#undef APP_VERSION
