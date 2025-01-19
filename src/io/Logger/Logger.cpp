#include "Logger.h"

#include <iostream>

namespace md::io {
    void md::io::Logger::initialize_logger() {
        // Create Console and File sink
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("log/logging.log", false);

        // Logger with both sinks
        auto logger = std::make_shared<spdlog::logger>("logger", spdlog::sinks_init_list{console_sink, file_sink});
        spdlog::set_default_logger(logger);
        logger->set_level(DEFAULT_LOG_LEVEL);

        // Set logging pattern
        // In Debug and Trace level, file name and line number is additionally displayed
        if (logger->level() == spdlog::level::debug || logger->level() == spdlog::level::trace) {
            logger->set_pattern("[%d.%m.%Y %H:%M:%S] [%^%l%$] [%s:%#] %v");
        }
        else {
            logger->set_pattern("[%d.%m.%Y %H:%M:%S] [%^%l%$] %v");
        }
    }
}  // namespace md::io
