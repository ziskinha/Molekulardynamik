#include "Parse.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#define RETURN_PARSE_ERROR(err_msg)                                                \
    SPDLOG_ERROR(err_msg);                                                         \
    SPDLOG_ERROR("For help, run the program with ./MolSim -h or ./MolSim --help"); \
    return ERROR;

#define PARSE_NUMBER(index, target, type, name)                                                                        \
    try {                                                                                                              \
        target = type(parameters[index]);                                                                              \
    } catch (const std::invalid_argument& e) {                                                                         \
        RETURN_PARSE_ERROR(fmt::format("Error while reading {} argument! Caught an invalid_argument exception", name)) \
    }

namespace md::parse {
    void displayHelp() {
        SPDLOG_INFO(
            "Displaying help information.\n"
            "Usage:\n"
            "  ./MolSim <input_file> <duration> <delta_t> <num_frames> <output_format>\n"
            "  ./MolSim -h | --help\n"
            "  ./MolSim -f\n"
            "  ./MolSim -b\n"
            "  ./MolSim -v\n\n"
            "Arguments:\n"
            "  input_file       Name of the file to read particle data from. Should end in .txt\n"
            "  duration         Simulation duration (e.g., 10.0).\n"
            "  delta_t          Time step delta_t (e.g., 0.01).\n"
            "  num_frames       Number of Frames saved (e.g. 500).\n"
            "  output_format    Output format: either 'XYZ' or 'VTK'.\n\n"
            "Flags:\n"
            "  -h, --help       Show this help message and exit.\n"
            "  -f               Delete all contents of the output folder before writing.\n"
            "  -b               Benchmark the simulation (output_format and output_folder optional).");
    }

    ParseStatus parse_args(int argc, char** argv, ProgramArguments& args) {
        SPDLOG_INFO("Start parsing arguments");
        std::vector<std::string> arguments(argv, argv + argc);

        // split arguments into flags & parameters
        std::vector<std::string> parameters;
        std::vector<std::string> flags;
        std::partition_copy(arguments.begin(), arguments.end(), std::back_inserter(flags),
                            std::back_inserter(parameters),
                            [](const std::string& arg) { return !arg.empty() && arg[0] == '-'; });

        auto flag_exists = [&](const std::string& option) -> bool {
            return std::find(flags.begin(), flags.end(), option) != flags.end();
        };

        // Check for help flag
        if (flag_exists("-h") || flag_exists("--help")) {
            displayHelp();
            return EXIT;
        }

        // check number of arguments
        // if -b set, expecting at least 4 arguments (filename, start_time, end_time, delta_t)
        // else expecting 6 arguments (filename, start_time, end_time, delta_t, fps, output_format)
        if ((flag_exists("-b") && parameters.size() < 4) || parameters.size() < 6) {
            RETURN_PARSE_ERROR(
                fmt::format("Error: Not enough arguments provided. Received {} arguments.", parameters.size()));
        }

        // set options
        args.benchmark = flag_exists("-b");
        args.override = flag_exists("-f");

        // parse arguments
        args.file = arguments[1];
        if (!std::filesystem::exists(args.file) || !std::filesystem::is_regular_file(args.file)) {
            RETURN_PARSE_ERROR(fmt::format("Error: File does not exist or is not a valid file.", args.file));
        }

        PARSE_NUMBER(2, args.duration, std::stod, "duration");
        PARSE_NUMBER(3, args.dt, std::stod, "delta_t");

        if (args.benchmark) {
            return OK;
        }

        PARSE_NUMBER(4, args.num_frames, std::stoi, "num_frames");

        if (parameters[5] != "XYZ" && parameters[5] != "VTK") {
            RETURN_PARSE_ERROR(fmt::format("Error: invalid file output format: {}", args.file));
        }
        args.output_format = parameters[5] == "XYZ" ? io::OutputFormat::XYZ : io::OutputFormat::VTK;

        SPDLOG_INFO("Arguments parsed successfully");
        return OK;
    }
}  // namespace md::parse
