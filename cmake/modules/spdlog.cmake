include(FetchContent)

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        v1.14.1
)

FetchContent_MakeAvailable(spdlog)
target_link_libraries(MolSim PRIVATE spdlog::spdlog)

# Set level of spdlog
if(SPDLOG_LEVEL STREQUAL "trace")
    add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
elseif(SPDLOG_LEVEL STREQUAL "debug" OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
elseif(SPDLOG_LEVEL STREQUAL "info")
    add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO)
elseif(SPDLOG_LEVEL STREQUAL "warn")
    add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_WARN)
elseif(SPDLOG_LEVEL STREQUAL "error")
    add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_ERROR)
elseif(SPDLOG_LEVEL STREQUAL "critical")
    add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_CRITICAL)
elseif(SPDLOG_LEVEL STREQUAL "off")
    add_compile_definitions(SPDLOG_ACTIVE_LEVEL=6)
else()
    add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO)
endif()