#pragma once
#include <spdlog/sinks/basic_file_sink.h>

#ifndef DEFAULT_LOG_LEVEL
    #define DEFAULT_LOG_LEVEL spdlog::level::info
#endif

#include "env/Common.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

/**
 * @brief Specialization of fmt::formatter for md::int3.
 */
template <>
struct fmt::formatter<md::int3> {
    /**
     * @brief Parses the format specifier.
     * @param ctx The format parse context.
     * @return The iterator pointing to the beginning of the parse context.
     */
    constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

    /**
     * @brief Formats the md::int3 into the output iterator.
     * @tparam FormatContext
     * @param vec The md::int3 object to format.
     * @param ctx The format context.
     * @return The formatted string, written to the output iterator.
     */
    template <typename FormatContext>
    auto format(const md::int3& vec, FormatContext& ctx) {
        return fmt::format_to(ctx.out(), "{{{}, {}, {}}}", vec[0], vec[1], vec[2]);
    }
};

/**
 * @brief Specialization of fmt::formatter for md::vec3.
 */
template <>
struct fmt::formatter<md::vec3> {
    /**
     * @brief Parses the format specifier.
     * @param ctx The format parse context.
     * @return The iterator pointing to the beginning of the parse context.
     */
    constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

    /**
     * @brief Formats the md::vec3 into the output iterator.
     * @tparam FormatContext
     * @param vec The md::vec3 object to format.
     * @param ctx The format context.
     * @return The formatted string, written to the output iterator.
     */
    template <typename FormatContext>
    auto format(const md::vec3& vec, FormatContext& ctx) {
        return fmt::format_to(ctx.out(), "{{{}, {}, {}}}", vec[0], vec[1], vec[2]);
    }
};

namespace md::io {
    /**
     * @brief A Logger class to handle logging functionalities. It initializes a file and console sink. Like that
     * log information is printed on the console and in a file.
     */
    class Logger {
       public:
        /**
         * @brief Initializes the logger for the simulation with a file and console sink. So log messages are displayed
         * on the console and are written to a log file. The log level is set at compile time with SPDLOG_LEVEL.
         */
        static void initialize_logger();
    };
}  // namespace md::io