# function to add test executables
function(add_test_executable test_name)
    file(GLOB TEST_SOURCES
            ${CMAKE_SOURCE_DIR}/testing/${test_name}.cpp
            ${CMAKE_SOURCE_DIR}/src/env/*.cpp
            ${CMAKE_SOURCE_DIR}/src/env/*.h
            ${CMAKE_SOURCE_DIR}/src/core/*.cpp
            ${CMAKE_SOURCE_DIR}/src/core/*.h
            ${CMAKE_SOURCE_DIR}/src/io/Output/CheckpointWriter.cpp
    )

    add_executable(${test_name} ${TEST_SOURCES})

    target_include_directories(${test_name}
            PUBLIC
            ${CMAKE_SOURCE_DIR}/libs/libxsd
            PRIVATE
            ${CMAKE_SOURCE_DIR}/src
    )

    target_link_libraries(${test_name}
            PUBLIC
            XercesC::XercesC
            gtest
            gtest_main
            PRIVATE
            spdlog::spdlog
    )

    add_dependencies(${test_name} gtest MolSim)
    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

# add the tests
add_test_executable(EnvironmentTests)
add_test_executable(ForceTests)
add_test_executable(StoermerVerletTests)
add_test_executable(LinkedCellsTests)
add_test_executable(BoundaryConditionTests)
add_test_executable(ThermostatTests)