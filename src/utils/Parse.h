#pragma once
#include "io/IOStrategy.h"
#include "io/Logger.h"
#include "../env/Environment.h"

namespace md::parse {

    enum ParseStatus { OK, EXIT, ERROR };

    using vec3 = std::array<double, 3>;

    /**
     * @brief Struct used to return arguments read from the terminal.
     */
    struct ProgramArguments {
        std::string file;
        std::string output_baseName;
        double duration;
        double dt;
        int write_freq;
        env::Environment env;
        std::string force;
        bool benchmark;
        bool override;
        io::OutputFormat output_format;
    };

    /**
     * @brief Logs the parsed program arguments
     * @param args
     */
    inline void log_arguments(const ProgramArguments& args) {
        SPDLOG_INFO(
            "Parsed Arguments:\n"
            "       output name:   {}\n"
            "       duration:      {}\n"
            "       dt:            {}\n"
            "       write_freq:    {}\n"
            "       particles:     {}\n" 
            "       force:         {}\n" 
            "       benchmark:     {}\n"
            "       override:      {}\n"
            "       output_format: {}",
            args.output_baseName, args.duration, args.dt, args.write_freq, args.env.size(), args.force, args.benchmark ? "true" : "false",
            args.override ? "true" : "false", args.output_format == io::OutputFormat::XYZ ? "XYZ" : "VTK");
    }

    /**
     * @brief Displays a help message with information about the usage of the program and describes arguments.
     */
    void displayHelp();

    /**
     * @brief Parses the arguments and executes the program.
     * @param argc
     * @param argv
     * @param args argument
     * @return ParseStatus indicating if there was an error or if the program should proceed
     */
    ParseStatus parse_args(int argc, char** argv, ProgramArguments& args);
}  // namespace md::parse
