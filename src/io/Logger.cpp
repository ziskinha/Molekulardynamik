#include "Logger.h"
#include <iostream>


namespace md::io {
    void md::io::Logger::initialize_logger(spdlog::level::level_enum log_lvl) {

        auto logger = spdlog::stdout_color_mt("console");
        logger->set_pattern("[%d.%m.%Y %H:%M:%S] [%^%l%$] %v");
        logger->set_level(log_lvl);

        spdlog::set_default_logger(logger);
    }
}

