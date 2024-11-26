#include <spdlog/sinks/basic_file_sink.h>

#include <memory>

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

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