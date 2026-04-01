#include "Logger.hpp"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using namespace utility;

Logger::Logger()
    : m_console(spdlog::stdout_color_mt("console_logger"))
    , m_file(spdlog::basic_logger_mt("file_logger", "log.txt"))
{
    spdlog::set_level(spdlog::level::trace);
    spdlog::set_pattern("[%l](%s:%#): %v");
}
