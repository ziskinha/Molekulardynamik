#include "Logger.h"
#include <iostream>

// TODO: Directory erstellen?

void md::io::Logger::initialize_logger(spdlog::level::level_enum log_lvl) {
    // TODO: log_lvl parsen

    auto logger = spdlog::stdout_color_mt("console");
    logger->set_level(log_lvl);
    logger->set_pattern("[%H:%M:%S] [%^%l%$] %v");

    spdlog::set_default_logger(logger);
}

