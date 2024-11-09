#include "Parse.h"
#include <iostream>
#include <filesystem>
#include <cstring>
#include <optional>
#include "io/FileReader.h"
#include "io/Logger.h"

namespace  md::parse {

	void Parse::displayHelp() {
        spdlog::info("Displaying help information");
		spdlog::info("Usage:\n"
			         "  ./MolSim filename end_time delta_t output_format\n"
                     "  ./MolSim -h\n"
			         "  ./MolSim --help\n"
			         "  ./MolSim -f\n"
                     "Arguments:\n"
                     "  filename         Name of a file from which the particle data is read\n"
                     "  end_time         End time for the simulation (e.g., 10.0)\n"
                     "  delta_t          Time step delta_t (e.g., 0.01)\n"
			         "  output_format    Defines output format: 0 for XYZ, 1 for VTK\n"
			         "  h, -help         Show this help message and exit\n"
                     "  f                Delete the ../cmake-build-debug/output directory with all its contents.\n");
	}

    std::optional <Parse::Parse_arguments> Parse::parse_args(const int argc, char *argv[]) {
		Parse_arguments arguments;
	spdlog::info("Welcome to our Molecular Simulation! :)");
    spdlog::info("Starting MolSim!");

	if (argc != 5) {
		if (argc == 2) {
			if (std::strcmp(argv[1], "-h") == 0 || std::strcmp(argv[1], "--help") == 0) {
				arguments.show_help=true;
				return arguments;
			}
			if (std::strcmp(argv[1], "-f") == 0 ) {
				std::filesystem::path to_delete= "../cmake-build-debug/output" ;
				try {
				    if (exists(to_delete) && is_directory(to_delete)) {
					    uintmax_t number_of_deleted_directories  = remove_all(to_delete);
                        spdlog::info("Deleted {} files from {}", number_of_deleted_directories - 1, to_delete.string());
					    arguments.delete_output=true;
					    return arguments;
				    }
				    spdlog::warn(R"(Output Directory does not exist. For more information run "./MolSim -h" or "./MolSim --help".)");
				    return std::nullopt;
                }
                catch (const std::filesystem:: filesystem_error& e) {
					spdlog::error("Error while deleting output directory: ", e.what());
					return std::nullopt;
				}
			}
			spdlog::warn("Erroneous program call!");
			displayHelp();
			return std::nullopt;
		}
        spdlog::warn("Erroneous program call!");
		displayHelp();
		return std::nullopt;
	}

    //Parse end_time
	try {
		arguments.end_time = std::stod(argv[2]);
        spdlog::info("Parsed end_time: {}", *arguments.end_time);
	}
	catch (const std::invalid_argument& e) {
		spdlog::error("Error reading end_time argument: {}", e.what());
        displayHelp();
		return std::nullopt;
	}
    catch (std::out_of_range& e) {
        spdlog::error("Error reading end_time argument (out of range): {}", e.what());
		return std::nullopt;
	}

    // Parse Δt
	try {
		arguments.delta_t = std::stod(argv[3]);
        spdlog::info("Parsed delta_t: {}", *arguments.delta_t);
	}
	catch (const std::invalid_argument& e) {
        spdlog::error("Error reading delta_t argument: {}", e.what());
        displayHelp();
		return std::nullopt;
	}
    catch (std::out_of_range& e) {
        spdlog::error("Error reading delta_t argument (out of range): {}", e.what());
        displayHelp();
		return std::nullopt;
	}

    //Parse output_format
	try {
        arguments.output_format = std::stoi(argv[4]);
        if (arguments.output_format) {
            spdlog::info("Parsed output_format: VTK");
        }
        else {
            spdlog::info("Parsed output_format: XYZ");
        }
	}
	catch (const std::invalid_argument& e) {
		spdlog::error("Error reading output_format: {}", e.what());
        displayHelp();
		return std::nullopt;
	}
    catch (std::out_of_range& e) {
        spdlog::error("Error reading output_format (out of range): {}", e.what());
		displayHelp();
		return std::nullopt;
	}

    //Parse filename
    try {
        arguments.file = argv[1];
        spdlog::info("Parsed filename: {}", arguments.file);
    }
    catch (std::exception& e) {
        spdlog::error("Error reading filename argument: {}", e.what());
        return std::nullopt;
    }

    spdlog::info("Arguments parsed successfully");
	return arguments;
    }

}
