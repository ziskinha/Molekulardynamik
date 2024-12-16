
#pragma once
#include "io/Logger/Logger.h"
#include "io/IOStrategy.h"

namespace md::parse {

    enum ParseStatus { OK, EXIT, ERROR };

    using vec3 = std::array<double, 3>;

    /**
     * @brief Displays a help message with information about the usage of the program and describes arguments.
     */
    void displayHelp();

    /**
     * @brief Parses the arguments and executes the program.
     * @param argc
     * @param argv
     * @param args arguments.
     * @return ParseStatus indicating if there was an error or if the program should proceed.
     */
    ParseStatus parse_args(int argc, char** argv, md::io::ProgramArguments& args);
}  // namespace md::parse
