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

	struct CuboidInfo {
		vec3 x;
		vec3 v;
		vec3 N;
		double h;
		double thermal_v;
		double mass;
		int type;
	};

	/**
	 * @brief Struct used to return arguments read from the terminal.
	 */
	struct ProgramArguments {
        std::string file;
        double duration;
        double dt;
        int num_frames;
        bool benchmark;
        bool delete_output_folder_contents;
        io::OutputFormat output_format;
    };

    inline void log_arguments(const ProgramArguments& args) {
        spdlog::info("Parsed Arguments:\n"
                     "       file:          {}\n"
                     "       duration:      {}\n"
                     "       dt:            {}\n"
                     "       num_frames:    {}\n"
                     "       benchmark:     {}\n"
                     "       override:      {}\n"
                     "       output_format: {}",
                     args.file,
                     args.duration,
                     args.dt,
                     args.num_frames,
                     args.benchmark ? "true" : "false",
                     args.delete_output_folder_contents ? "true" : "false",
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
      * @param args
      */
	ParseStatus parse_args(int argc, char** argv, ProgramArguments& args);
}


