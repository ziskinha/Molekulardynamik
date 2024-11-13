#pragma once
#include "io/IOStrategy.h"
#include "io/Logger.h"


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
	};

    /**
     * @brief Logs the parsed program arguments
     * @param args
     */
    inline void log_arguments(const ProgramArguments& args) {
        spdlog::info("Parsed Arguments:\n"
                     "       file:          {}\n"
                     "       duration:      {}\n"
                     "       dt:            {}\n"
                     "       num_frames:    {}\n"
                     "       benchmark:     {}\n"
                     "       override:      {}\n"
                     "       verbose:       {}\n"
                     "       output_format: {}",
                     args.file,
                     args.duration,
                     args.dt,
                     args.num_frames,
                     args.benchmark ? "true" : "false",
                     args.override ? "true" : "false",
                     args.verbose ? "true" : "false",
                     args.output_format == io::OutputFormat::XYZ ? "XYZ" : "VTK");
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
}


