#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <memory>

/**
 * @brief A Logger class to handle logging functionalities
 */
namespace md::io {
    class Logger {
    public:
        /**
         * @brief Default constructor for Logger class
         */
        Logger() = default;
        /**
         * @brief Default destructor for Logger class
         */
        ~Logger() = default;

        /**
         * @brief Initializes the logger with the specific log level and sets the pattern
         * @param log_lvl
         */
        static void initialize_logger(spdlog::level::level_enum log_lvl);
    };
}