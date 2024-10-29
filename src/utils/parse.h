#pragma once


namespace  md ::Parse {

    /**
     * @brief Parses arguments from the terminal.
     */
    class parse {
    public:

        /**
        * @brief Displays a help message with information about the usage of the program and describes arguments.
        */
        void displayHelp();

        /**
         * @brief Parses the arguments and executes the program.
         * @param argc
         * @param argv
         */
        int parse_args( int argc, char* argv[]);
    };
}

