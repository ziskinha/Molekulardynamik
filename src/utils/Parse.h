#pragma once
#include <optional>
#include "io/OutputStrategy.h"


namespace md::parse {

    /**
     * @brief Parses arguments from the terminal.
     */
    class Parse {
    public:

        /**
               * @brief Struct used to return arguments read from the terminal.
               */
        struct Parse_arguments {
            std::vector<Particle> file;
            std::optional<double> end_time;
            std::optional<double> delta_t;
            std::optional<bool> output_format;
            bool show_help = false;
            bool delete_output = false;
        };

        /**
        * @brief Displays a help message with information about the usage of the program and describes arguments.
        */
        void displayHelp();

        /**
         * @brief Parses the arguments and executes the program.
         * @param argc
         * @param argv
         */
        std::optional <Parse_arguments> parse_args( int argc, char* argv[]);
    };
}

