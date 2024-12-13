include(FetchContent)

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        v1.14.1
)

FetchContent_MakeAvailable(spdlog)
target_link_libraries(MolSim PRIVATE spdlog::spdlog)

# Set level of spdlog
# DEFAULT_LOG_LEVEL is required to create a logger with both file and console sinks and to set their logging patterns
# Else the log level would be set to info when creating the logger
if(SPDLOG_LEVEL STREQUAL "trace")
    target_compile_definitions(MolSim PUBLIC SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
    target_compile_definitions(MolSim PUBLIC DEFAULT_LOG_LEVEL=spdlog::level::trace)
elseif(SPDLOG_LEVEL STREQUAL "debug" OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(MolSim PUBLIC SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
    target_compile_definitions(MolSim PUBLIC DEFAULT_LOG_LEVEL=spdlog::level::debug)
elseif(SPDLOG_LEVEL STREQUAL "info")
    target_compile_definitions(MolSim PUBLIC SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO)
    target_compile_definitions(MolSim PUBLIC DEFAULT_LOG_LEVEL=spdlog::level::info)
elseif(SPDLOG_LEVEL STREQUAL "warn")
    target_compile_definitions(MolSim PUBLIC SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_WARN)
    target_compile_definitions(MolSim PUBLIC DEFAULT_LOG_LEVEL=spdlog::level::warn)
elseif(SPDLOG_LEVEL STREQUAL "error")
    target_compile_definitions(MolSim PUBLIC SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_ERROR)
    target_compile_definitions(MolSim PUBLIC DEFAULT_LOG_LEVEL=spdlog::level::error)
elseif(SPDLOG_LEVEL STREQUAL "critical")
    target_compile_definitions(MolSim PUBLIC SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_CRITICAL)
    target_compile_definitions(MolSim PUBLIC DEFAULT_LOG_LEVEL=spdlog::level::critical)
elseif(SPDLOG_LEVEL STREQUAL "off")
    target_compile_definitions(MolSim PUBLIC SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_OFF)
    target_compile_definitions(MolSim PUBLIC DEFAULT_LOG_LEVEL=spdlog::level::off)
else()
    target_compile_definitions(MolSim PUBLIC SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO)
endif()