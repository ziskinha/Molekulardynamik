#include <algorithm>
#include <string>
#include <vector>

#include "Parse.h"
#include "io/IOStrategy.h"

#define RETURN_PARSE_ERROR(err_msg)                                                \
    SPDLOG_ERROR(err_msg);                                                         \
    SPDLOG_ERROR("For help, run the program with ./MolSim -h or ./MolSim --help"); \
    return ERROR;

namespace md::parse {
    void displayHelp() {
        SPDLOG_INFO(
            "Displaying help information.\n"
            "Usage:\n"
            "  ./MolSim <xml_file> <output_format>\n"
            "  ./MolSim -h | --help\n"
            "Arguments:\n"
            "  input_file       XML or TXT file with input parameters important for the simulation.\n"
            "  output_format    Output format: either 'XYZ' or 'VTK'.\n\n"
            "Flags:\n"
            "  -h, --help       Show this help message and exit.\n"
            "  -f               Delete all contents of the output folder before writing.\n"
            "  -b               Benchmark the simulation (output_format and output_folder optional).");
    }

    ParseStatus parse_args(int argc, char** argv, io::ProgramArguments& args) {
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
        // if -b set, expecting at least 2 arguments (filename, xml file)
        // else expecting 3 arguments (filename, xml file, output_format)
        if ((flag_exists("-b") && parameters.empty()) || parameters.size() < 2) {
            RETURN_PARSE_ERROR(
                fmt::format("Not enough arguments provided. Received {} arguments", parameters.size()));
        }

        io::read_file(arguments[1], args);

        // set options
        args.benchmark = flag_exists("-b");
        args.override = flag_exists("-f");

        if (args.benchmark) {
            args.output_format = io::OutputFormat::XYZ;
            return OK;
        }

        if (parameters[2] != "XYZ" && parameters[2] != "VTK") {
            RETURN_PARSE_ERROR(fmt::format("Invalid file output format: {}", parameters[2]));
        }
        args.output_format = parameters[2] == "XYZ" ? io::OutputFormat::XYZ : io::OutputFormat::VTK;

        SPDLOG_INFO("Arguments successfully parsed");
        io::log_arguments(args);
        return OK;
    }
}  // namespace md::parse