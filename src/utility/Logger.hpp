#pragma once

namespace utility {
class Logger : public utility::Singleton<Logger>
{
    friend class utility::Singleton<Logger>;

private:
    std::shared_ptr<spdlog::logger> m_console;
    std::shared_ptr<spdlog::logger> m_file;

private:
    Logger();
    ~Logger() = default;

public:
    static std::shared_ptr<spdlog::logger> GetConsoleLogger() { return GetInstance().m_console; }
    static std::shared_ptr<spdlog::logger> GetFileLogger() { return GetInstance().m_file; }
};
}

#define LOGI(fmt, ...)                                      \
    do {                                                    \
        SPDLOG_LOGGER_INFO(utility::Logger::GetConsoleLogger(), fmt, \
            ##__VA_ARGS__);                                 \
        SPDLOG_LOGGER_INFO(utility::Logger::GetFileLogger(), fmt,    \
            ##__VA_ARGS__);                                 \
    } while (0)

#define LOGE(fmt, ...)                                       \
    do {                                                     \
        SPDLOG_LOGGER_ERROR(utility::Logger::GetConsoleLogger(), fmt, \
            ##__VA_ARGS__);                                  \
        SPDLOG_LOGGER_ERROR(utility::Logger::GetFileLogger(), fmt,    \
            ##__VA_ARGS__);                                  \
    } while (0)

#define LOGW(fmt, ...)                                      \
    do {                                                    \
        SPDLOG_LOGGER_WARN(utility::Logger::GetConsoleLogger(), fmt, \
            ##__VA_ARGS__);                                 \
        SPDLOG_LOGGER_WARN(utility::Logger::GetFileLogger(), fmt,    \
            ##__VA_ARGS__);                                 \
    } while (0)

#define LOGD(fmt, ...)                                       \
    do {                                                     \
        SPDLOG_LOGGER_DEBUG(utility::Logger::GetConsoleLogger(), fmt, \
            ##__VA_ARGS__);                                  \
        SPDLOG_LOGGER_DEBUG(utility::Logger::GetFileLogger(), fmt,    \
            ##__VA_ARGS__);                                  \
    } while (0)

#define LOGC(fmt, ...)                                          \
    do {                                                        \
        SPDLOG_LOGGER_CRITICAL(utility::Logger::GetConsoleLogger(), fmt, \
            ##__VA_ARGS__);                                     \
        SPDLOG_LOGGER_CRITICAL(utility::Logger::GetFileLogger(), fmt,    \
            ##__VA_ARGS__);                                     \
    } while (0)

#define LOGT(fmt, ...)                                       \
    do {                                                     \
        SPDLOG_LOGGER_TRACE(utility::Logger::GetConsoleLogger(), fmt, \
            ##__VA_ARGS__);                                  \
        SPDLOG_LOGGER_TRACE(utility::Logger::GetFileLogger(), fmt,    \
            ##__VA_ARGS__);                                  \
    } while (0)
