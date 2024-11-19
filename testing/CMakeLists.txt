# Define the test executable
add_executable(ParticleContainerTests
        ${CMAKE_SOURCE_DIR}/src/Environment.cpp
        ${CMAKE_SOURCE_DIR}/testing/ParticleContainerTests.cpp
        ../src/Environment.h
)

# Define the test executable
add_executable(ForceTests
        ${CMAKE_SOURCE_DIR}/src/force.h
        ${CMAKE_SOURCE_DIR}/src/Environment.cpp
        ${CMAKE_SOURCE_DIR}/testing/ForceTests.cpp
        ../src/Environment.h
)

# Define the test executable
add_executable(StoermerVerletTests
        ${CMAKE_SOURCE_DIR}/src/force.h
        ${CMAKE_SOURCE_DIR}/src/Environment.cpp
        ${CMAKE_SOURCE_DIR}/src/StoermerVerlet.cpp
        ${CMAKE_SOURCE_DIR}/testing/StoermerVerletTests.cpp
        ${CMAKE_SOURCE_DIR}/src/IntegratorBase.cpp
        ../src/Environment.h
)

#include directories to test executables
target_include_directories(ParticleContainerTests
        PUBLIC
        ${CMAKE_SOURCE_DIR}/libs/libxsd
        PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

target_include_directories(ForceTests
        PUBLIC
        ${CMAKE_SOURCE_DIR}/libs/libxsd
        PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

target_include_directories(StoermerVerletTests
        PRIVATE
        ${CMAKE_SOURCE_DIR}/external/spdlog/include
        PUBLIC
        ${CMAKE_SOURCE_DIR}/libs/libxsd
        PRIVATE
        ${CMAKE_SOURCE_DIR}/src
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