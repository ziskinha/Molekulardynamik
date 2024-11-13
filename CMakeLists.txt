cmake_minimum_required(VERSION 3.10)

# define project name, version
project(PSEMolDyn_GroupE VERSION 0.0.1)

# let ccmake and cmake-gui offer the default build type options
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release;RelWithDebInfo;MinSizeRel")

# set Release as the default build type if it is not yet set.
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif (NOT CMAKE_BUILD_TYPE)

# set optimization flags for release build
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2" CACHE STRING "Release build optimization level" FORCE)
endif ()

# Enable AdressSanitizer for Debug builds (Continuous Integration)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
endif ()

# collect all cpp files
file(GLOB_RECURSE MY_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        # header don't need to be included but this might be necessary for some IDEs
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

# create make target
add_executable(MolSim ${MY_SRC}
        src/IntegratorBase.cpp
        src/IntegratorBase.h
        src/StoermerVerlet.cpp
        src/StoermerVerlet.h
        src/io/IOStrategy.h
        src/io/FileReader.h
        src/io/FileReader.cpp
        src/force.h
        src/utils/Parse.cpp
        src/utils/Parse.h
        src/RungeKutta.h
        src/io/Logger.cpp
        src/io/Logger.h
)

# set cxx standard. You may raise this if you want.
target_compile_features(MolSim
        PRIVATE
        cxx_std_17
)

target_include_directories(MolSim
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/libxsd
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(MolSim
        # stuff that is used in headers and source files
        PUBLIC
        xerces-c
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

enable_testing()

# Define the test executable
add_executable(ParticleContainerTests
        src/Particle.cpp
        testing/ParticleContainerTests.cpp
)

# Define the test executable
add_executable(ForceTests
        src/force.h
        src/Particle.cpp
        testing/ForceTests.cpp
)

# Define the test executable
add_executable(StoermerVerletTests
        src/force.h
        src/Particle.cpp
        src/StoermerVerlet.cpp
        testing/StoermerVerlet.cpp
        src/IntegratorBase.cpp
)

#include directories to test executables
target_include_directories(ParticleContainerTests
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/libxsd
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(ForceTests
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/libxsd
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(StoermerVerletTests
        PRIVATE 
        ${CMAKE_SOURCE_DIR}/external/spdlog/include
       PUBLIC
       ${CMAKE_CURRENT_SOURCE_DIR}/libs/libxsd
     PRIVATE
       ${CMAKE_CURRENT_SOURCE_DIR}/src      
)

#link liblaries to test executables
target_link_libraries(ParticleContainerTests
        PUBLIC
        xerces-c
        gtest
        gtest_main
        PRIVATE 
        spdlog::spdlog
)

target_link_libraries(ForceTests
        PUBLIC
        xerces-c
        gtest
        gtest_main
        PRIVATE 
        spdlog::spdlog
)
target_link_libraries(StoermerVerletTests
        PUBLIC
        xerces-c
        gtest
        gtest_main
        PRIVATE 
        spdlog::spdlog
)

add_dependencies(ParticleContainerTests gtest MolSim)
add_dependencies(ForceTests gtest MolSim)
add_dependencies(StoermerVerletTests gtest MolSim)

# Register the tests
add_test(NAME PartContTests COMMAND ParticleContainerTests)
add_test(NAME ForceTests COMMAND ForceTests)
add_test(NAME StoermerVerletTests COMMAND StoermerVerletTests)