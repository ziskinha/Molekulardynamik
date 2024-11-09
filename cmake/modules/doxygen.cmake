# add Doxygen
find_package(Doxygen)

# option to disable creating the Doxygen target
option(ENABLE_DOXYGEN "Creating Doxygen target" ON)

if (ENABLE_DOXYGEN AND DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)

    add_custom_target(doc_doxygen
            ${DOXYGEN_EXECUTABLE} ${DOXYGEN_IN}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating Doxygen documentation"
            VERBATIM)
endif ()