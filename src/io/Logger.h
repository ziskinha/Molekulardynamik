#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace md::io {
    class Logger {
    public:
        static void initialize_logger(spdlog::level::level_enum log_lvl);
    };
}