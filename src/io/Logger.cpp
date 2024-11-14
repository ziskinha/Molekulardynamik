#include "Logger.h"
#include <iostream>


namespace md::io {
    void md::io::Logger::initialize_logger() {

        auto logger = spdlog::stdout_color_mt("console");
        //Set logging pattern
        // to additionaly display file name and #line, add [%s:%#]
        logger->set_pattern("[%d.%m.%Y %H:%M:%S] [%^%l%$] %v");

        spdlog::set_default_logger(logger);

        //set spdlog level to debug or trace
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
    }
}

