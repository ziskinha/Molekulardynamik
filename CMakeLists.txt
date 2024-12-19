cmake_minimum_required(VERSION 3.10)

# define project name, version
project(PSEMolDyn_GroupE VERSION 0.0.1)

# let cmake and cmake-gui offer the following build type options
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release;Benchmark;Profile")

# Compiler-specific flags
if (CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    set(COMPILER_OPTIM_FLAGS "-O3 -ipo -xHost -qopenmp")
    set(DEBUG_FLAGS "-g -debug inline-debug-info -traceback")
    set(WARNING_FLAGS "-w3 -wd383,981,1418,1572,2259")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(COMPILER_OPTIM_FLAGS "-Ofast -march=native -flto -funroll-loops")
    set(DEBUG_FLAGS "-g -fsanitize=address -fsanitize=undefined")
    set(WARNING_FLAGS "-Wall -Wextra -Wpedantic")
else()
    set(COMPILER_OPTIM_FLAGS "-O2")
    set(DEBUG_FLAGS "-g")
    set(WARNING_FLAGS "-Wall")
endif()

# Build type-specific flags
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DEBUG_FLAGS} -DDEBUG")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${DEBUG_FLAGS}")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_OPTIM_FLAGS}")
elseif (CMAKE_BUILD_TYPE STREQUAL "Benchmark")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_OPTIM_FLAGS}")
    add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_OFF)
elseif (CMAKE_BUILD_TYPE STREQUAL "Profile")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg ${COMPILER_OPTIM_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
    add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_OFF)
endif()

# -Ofast: Most aggressive optimization level
# -march=native: Enable instructions for the local machine architecture
# -mtune=native: Optimize for the local machine's performance
# -flto: Enable link-time optimization
# -funroll-loops: Unroll loops for better performance
# -finline-functions: Aggressively inline small and medium-sized functions

# collect all cpp files
file(GLOB_RECURSE MY_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/io/*.cxx" 
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/io/*.hxx" 
)

# create make target
add_executable(MolSim ${MY_SRC})

# set cxx standard. You may raise this if you want.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

target_compile_features(MolSim
        PRIVATE
        cxx_std_23
)

target_include_directories(MolSim
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/ankerl
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/libxsd
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

find_package(XercesC REQUIRED)

target_link_libraries(MolSim
        # stuff that is used in headers and source files
        PUBLIC
        #xerces-c
        XercesC::XercesC
)

# activate all compiler warnings. Clean up your code :P
# depending on the compiler different flags are used
target_compile_options(MolSim
        PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
        # disable some spam warnings for icpc...
        $<$<CXX_COMPILER_ID:Intel>:-w3 -wd383,981,1418,1572,2259>

        $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

include(doxygen)
include(spdlog)
include(googletest)

# Enable testing and a separate testing directory with its own CMakeLists.txt
if (NOT (CMAKE_BUILD_TYPE STREQUAL "Benchmark"))
    enable_testing()
    add_subdirectory(testing)
endif()


