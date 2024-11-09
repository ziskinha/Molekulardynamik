#pragma once
#include "io/IOStrategy.h"


namespace md::parse {


	enum ParseStatus {
		OK,
		EXIT,
		ERROR
	};

	using vec3 = std::array<double, 3>;

	/**
	 * @brief Struct used to return arguments read from the terminal.
	 */
	struct ProgramArguments {
		std::string file;
		double duration;
		double dt;
		int num_frames;
		bool benchmark;
		bool override;
		bool verbose;
		io::OutputFormat output_format;

		friend std::ostream& operator<<(std::ostream& os, const ProgramArguments& args) {
			os << "Parsed Arguments:\n"
				<< "  file: " << args.file << "\n"
				<< "  duration: " << args.duration << "\n"
				<< "  dt: " << args.dt << "\n"
				<< "  num_frames: " << args.num_frames << "\n"
				<< "  benchmark: " << args.benchmark << "\n"
				<< "  override: " << args.override << "\n"
				<< "  verbose: " << args.verbose << "\n"
				<< "  output_format: " << (args.output_format == io::OutputFormat::XYZ ? "XYZ" : "VTK") << "\n";
			return os;
		}
	};

     /**
     * @brief Displays a help message with information about the usage of the program and describes arguments.
     */
	void displayHelp();


     /**
      * @brief Parses the arguments and executes the program.
      * @param argc
      * @param argv
      * @param args
      */
	ParseStatus parse_args(int argc, char** argv, ProgramArguments& args);
}


