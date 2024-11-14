#include "Logger.h"
#include <iostream>


namespace md::io {
    void md::io::Logger::initialize_logger() {

        //Console sink
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_pattern("[%d.%m.%Y %H:%M:%S] [%^%l%$] %v");

        //File sink
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("log/logging.log", false);
        file_sink->set_pattern("[%d.%m.%Y %H:%M:%S] [%^%l%$] %v");

        //Logger with both sinks
        auto logger = std::make_shared<spdlog::logger>("logger", spdlog::sinks_init_list{console_sink, file_sink});
        spdlog::set_default_logger(logger);

        //set spdlog level to debug or trace
        #ifdef SPDLOG_ACTIVE_LEVEL
            if (SPDLOG_ACTIVE_LEVEL == SPDLOG_LEVEL_DEBUG) {
                logger->set_level(spdlog::level::debug);
                // In Debug mode, file name and line number is additionally displayed
                logger->set_pattern("[%d.%m.%Y %H:%M:%S] [%^%l%$] [%s:%#]%v");
                SPDLOG_INFO("Log level set to debug");
            }
            else if (SPDLOG_ACTIVE_LEVEL == SPDLOG_LEVEL_TRACE) {
                logger->set_level(spdlog::level::trace);
                SPDLOG_INFO("Log level set to trace");
            }
        #endif
    }
}

