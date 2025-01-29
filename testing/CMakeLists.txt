# function to add test executables
function(add_test_executable test_name use_input)
    file(GLOB TEST_SOURCES
            ${CMAKE_SOURCE_DIR}/testing/${test_name}.cpp
            ${CMAKE_SOURCE_DIR}/src/env/*.cpp
            ${CMAKE_SOURCE_DIR}/src/env/*.h
            ${CMAKE_SOURCE_DIR}/src/core/IntegratorBase.cpp
            ${CMAKE_SOURCE_DIR}/src/core/IntegratorBase.h
            ${CMAKE_SOURCE_DIR}/src/core/StoermerVerlet/*.cpp
            ${CMAKE_SOURCE_DIR}/src/core/StoermerVerlet/*.h
            ${CMAKE_SOURCE_DIR}/src/effects/*.cpp
            ${CMAKE_SOURCE_DIR}/src/effects/*.h
            ${CMAKE_SOURCE_DIR}/src/io/Output/CheckpointWriter.cpp
    )

    if(${use_input})
        list(APPEND TEST_SOURCES
            ${CMAKE_SOURCE_DIR}/src/io/IOStrategy.h
            ${CMAKE_SOURCE_DIR}/src/io/IOStrategy.cpp
            ${CMAKE_SOURCE_DIR}/src/io/input/txt/TXTFileReader.cpp
            ${CMAKE_SOURCE_DIR}/src/io/input/xml/XMLFileReader.cpp
            ${CMAKE_SOURCE_DIR}/src/io/input/xml/molSimSchema.cxx
            ${CMAKE_SOURCE_DIR}/src/io/Output/VTKWriter.cpp
            ${CMAKE_SOURCE_DIR}/src/io/Output/XYZWriter.cpp
            ${CMAKE_SOURCE_DIR}/src/io/Output/CSVWriter.cpp
            ${CMAKE_SOURCE_DIR}/src/io/Output/VTK\ unstructured/vtk-unstructured.cpp
            ${CMAKE_SOURCE_DIR}/src/core/Statistics.cpp
        )
    endif()

    add_executable(${test_name} ${TEST_SOURCES})

    target_include_directories(${test_name}
            PUBLIC
            ${CMAKE_SOURCE_DIR}/libs/ankerl
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
add_test_executable(EnvironmentTests OFF)
add_test_executable(ForceTests OFF)
add_test_executable(StoermerVerletTests OFF)
add_test_executable(LinkedCellsTests OFF)
add_test_executable(BoundaryConditionTests OFF)
add_test_executable(ThermostatTests OFF)
add_test_executable(ParallelizationTests OFF)
add_test_executable(IOTests ON)