#include <spdlog/sinks/basic_file_sink.h>

#include <memory>

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"


#define UINT_T uint32_t
#define INT_T int64_t
using vec3 = std::array<double, 3>;
using uint3 = std::array<UINT_T, 3>;
using int3 = std::array<INT_T, 3>;

template <> struct fmt::formatter<int3> {
    // Parses the format specifier
    constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

    // Formats the int3 into the output iterator
    template <typename FormatContext>
    auto format(const int3& vec, FormatContext& ctx) {
        return fmt::format_to(ctx.out(), "{{{}, {}, {}}}", vec[0], vec[1], vec[2]);
    }
};

template <> struct fmt::formatter<vec3> {
    // Parses the format specifier
    constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

    // Formats the int3 into the output iterator
    template <typename FormatContext>
    auto format(const vec3& vec, FormatContext& ctx) {
        return fmt::format_to(ctx.out(), "{{{}, {}, {}}}", vec[0], vec[1], vec[2]);
    }
};

/**
 * @brief A Logger class to handle logging functionalities. It initializes a file and console sink. Like that
 * log information is printed on the console and in a file.
 */
namespace md::io {

    class Logger {
       public:
        /**
         * @brief Initializes the logger with the specific log level and sets the pattern
         * @param log_lvl
         */
        static void initialize_logger();
    };
}  // namespace md::io