include(FetchContent)

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        v1.14.1
)

FetchContent_MakeAvailable(spdlog)
target_link_libraries(MolSim PRIVATE spdlog::spdlog)