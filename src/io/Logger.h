#pragma once
#include <spdlog/sinks/basic_file_sink.h>

// #include <memory>

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include "env/Common.h"



template <> struct fmt::formatter<md::int3> {
    // Parses the format specifier
    constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

    // Formats the int3 into the output iterator
    template <typename FormatContext>
    auto format(const md::int3& vec, FormatContext& ctx) {
        return fmt::format_to(ctx.out(), "{{{}, {}, {}}}", vec[0], vec[1], vec[2]);
    }
};

template <> struct fmt::formatter<md::vec3> {
    // Parses the format specifier
    constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

    // Formats the int3 into the output iterator
    template <typename FormatContext>
    auto format(const md::vec3& vec, FormatContext& ctx) {
        return fmt::format_to(ctx.out(), "{{{}, {}, {}}}", vec[0], vec[1], vec[2]);
    }
};

/**
 * @brief A Logger class to handle logging functionalities.
 */
namespace md::io {

    class Logger {
       public:
        /**
         * @brief Initializes the logger for the simulation with a file and console sink. So log messages are displayed
         * on the console and are written to a log file. The log level is set at compile time with SPDLOG_LEVEL.
         */
        static void initialize_logger();
    };
}  // namespace md::io