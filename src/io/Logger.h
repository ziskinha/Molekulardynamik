#include <spdlog/sinks/basic_file_sink.h>

#include <memory>

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

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