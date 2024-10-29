//
// Created by grz on 10/29/24.
//

#include "parse.h"
#include <iostream>
#include <filesystem>
#include "force.hpp"
#include "io/FileReader.h"
#include "io/OutputStrategy.h"
#include "StoermerVerlet.h"
#include "Particle.h"
#include <cstring>



namespace  md::Parse {

	void parse::displayHelp() {
		std::cout << "Usage:\n"
			<< "  ./MolSim filename end_time delta_t output_format\n"
			<< "  ./MolSim -h\n"
			<< "  ./MolSim --help\n"
			<< "   ./MolSim -f\n"
			<< "Arguments:\n"
			<< "  filename         Name of a file from which the particle data is read\n"
			<< "  end_time         End time for the simulation (e.g., 10.0)\n"
			<< "  delta_t          Time step delta_t (e.g., 0.01)\n"
			<< "  output_format    Defines output format: 0 for XYZ, 1 for VTK\n"
			<< "  -h, --help       Show this help message and exit\n"
			<<"   -f               Delete the ../cmake-build-debug/output directory with all its contents";
	}

    int parse::parse_args(const int argc, char *argv[]) {
    	double end_time;
	double delta_t;
	bool output_format;
	std::cout << "Hello from MolSim for PSE!" << std::endl;

	if (argc != 5) {
		if (argc == 2) {
			if (std::strcmp(argv[1], "-h") == 0 || std::strcmp(argv[1], "--help") == 0) {
				displayHelp();
				return 0;
			}
			if (std::strcmp(argv[1], "-f") == 0 ) {
				std::filesystem::path to_delete= "../cmake-build-debug/output" ;
				try {
				if (exists(to_delete) && is_directory(to_delete)) {
					uintmax_t number_of_deleted_directories  = remove_all(to_delete);
					std::cout << "Deleted " << number_of_deleted_directories -1 << " files from " << to_delete << "\n";
					return 0;
				}
				std::cout << "Output Directory does not exist. For more information run \"./MolSim -h\" or \"./MolSim --help\".\n";
				return 0;
				} catch (const std::filesystem:: filesystem_error& e) {
					std::cerr << "Error while deleting output directory: " << e.what() << '\n';
					return 0;
				}
			}
			std::cout << "Erroneous programme call! " << std::endl;
			displayHelp();
			return 1;
		}
		std::cout << "Erroneous programme call! " << std::endl;
		displayHelp();
		return 1;
	}


	try {
		end_time = std::stod(argv[2]);
	}
	catch (const std::invalid_argument& e) {
		std::cerr << "Error while reading end_time argument! Caught a invalid_argument exception: " << e.what() <<
			std::endl << "for help run the program with ./MolSim -h or ./MolSim --help" << std::endl;
		return 0;
	}catch (std::out_of_range& e) {
		std::cerr << "Error while reading end_time argument! Caught a out_of_range exception: " << e.what() << std::endl
			<< "for help run the program with ./MolSim -h or ./MolSim --help" << std::endl;
		return 0;
	}

	try {
		delta_t = std::stod(argv[3]);
	}
	catch (const std::invalid_argument& e) {
		std::cerr << "Error while reading delta_t argument! Caught a invalid_argument exception: " << e.what() <<
			std::endl << "for help run the program with ./MolSim -h or ./MolSim --help" << std::endl;
		return 0;
	}catch (std::out_of_range& e) {
		std::cerr << "Error while reading delta_t argument! Caught a out_of_range exception: " << e.what() << std::endl
			<< "for help run the program with ./MolSim -h or ./MolSim --help" << std::endl;
		return 0;
	}

	try {
        output_format = std::stoi(argv[4]);
	}
	catch (const std::invalid_argument& e) {
		std::cerr << "Error while reading output_format argument! Caught a invalid_argument exception: " << e.what() <<
			std::endl << "for help run the program with ./MolSim -h or ./MolSim --help" << std::endl;
		return 0;
	}catch (std::out_of_range& e) {
		std::cerr << "Error while reading output_format argument! Caught a out_of_range exception: " << e.what() << std::endl
			<< "for help run the program with ./MolSim -h or ./MolSim --help" << std::endl;
		return 0;
	}

	constexpr double start_time = 0;
	std::vector<Particle> file;
	try {
		file = io::FileReader::read_file(argv[1]);
	}
	catch (std::exception& e) {
		std::cerr << "Error while reading filename argument! " << e.what() << std::endl <<
			"for help run the program with ./MolSim -h or ./MolSim --help" << std::endl;
		return 0;
	}

	ParticleContainer particles(file);
	const auto writer = io::createWriter(output_format, particles.size());

	Integrator::StoermerVerlet simulator(particles, force::inverse_square(), *writer);
	simulator.simulate(start_time, end_time, delta_t, 200);

	std::cout << "output written. Terminating..." << std::endl;
	return 0;
    }

}
