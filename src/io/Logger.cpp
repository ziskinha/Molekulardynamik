#include "Logger.h"
#include <iostream>


namespace md::io {
    void md::io::Logger::initialize_logger() {

        auto logger = spdlog::stdout_color_mt("console");
        logger->set_pattern("[%d.%m.%Y %H:%M:%S] [%^%l%$] %v");

        #ifdef SPDLOG_ACTIVE_LEVEL
            if (SPDLOG_ACTIVE_LEVEL == SPDLOG_LEVEL_DEBUG) {
                logger->set_level(spdlog::level::debug);
                SPDLOG_INFO("Log level set to debug");
            }
            else if (SPDLOG_ACTIVE_LEVEL == SPDLOG_LEVEL_TRACE) {
                logger->set_level(spdlog::level::trace);
                SPDLOG_INFO("Log level set to trace");
            }
        #endif

        spdlog::set_default_logger(logger);
    }
}

