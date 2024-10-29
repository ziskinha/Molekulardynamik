#include <iostream>


#include "force.hpp"
#include "io/FileReader.h"
#include "io/OutputStrategy.h"

#include "StoermerVerlet.h"
#include "Particle.h"
#include "RungeKutta.h"

#include <cstring>

void displayHelp();

int main(const int argc, char* argv[]) {
	double end_time;
	double delta_t;
	bool useVTK;
	std::cout << "Hello from MolSim for PSE!" << std::endl;

	if (argc != 5) {
		if (argc == 2) {
			if (std::strcmp(argv[1], "-h") == 0 || std::strcmp(argv[1], "--help") == 0) {
				displayHelp();
				return 0;
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
		useVTK = std::stoi(argv[4]);
	}
	catch (const std::invalid_argument& e) {
		std::cerr << "Error while reading useVTK argument! Caught a invalid_argument exception: " << e.what() <<
			std::endl << "for help run the program with ./MolSim -h or ./MolSim --help" << std::endl;
		return 0;
	}catch (std::out_of_range& e) {
		std::cerr << "Error while reading useVTK argument! Caught a out_of_range exception: " << e.what() << std::endl
			<< "for help run the program with ./MolSim -h or ./MolSim --help" << std::endl;
		return 0;
	}

	constexpr double start_time = 0;
	std::vector<md::Particle> file;
	try {
		file = md::io::FileReader::read_file(argv[1]);
	}
	catch (std::exception& e) {
		std::cerr << "Error while reading filename argument! " << e.what() << std::endl <<
			"for help run the program with ./MolSim -h or ./MolSim --help" << std::endl;
		return 0;
	}

	md::ParticleContainer particles(file);
	const auto writer = md::io::createWriter(useVTK, particles.size());

	md::Integrator::StoermerVerlet simulator(particles, md::force::inverse_square(), *writer);
	simulator.simulate(start_time, end_time, delta_t, 600);

	std::cout << "output written. Terminating..." << std::endl;
	return 0;
}

void displayHelp() {
	std::cout << "Usage:\n"
		<< "  ./MolSim filename end_time delta_t useVTK\n"
		<< "  ./MolSim -h\n"
		<< "  ./MolSim --help\n\n"
		<< "Arguments:\n"
		<< "  filename         Name of a file from which the particle data is read\n"
		<< "  end_time         End time for the simulation (e.g., 10.0)\n"
		<< "  delta_t          Time step delta_t (e.g., 0.01)\n"
		<< "  useVTK           Use VTK output: 1 to enable, 0 to disable\n"
		<< "  -h, --help       Show this help message and exit\n";
}
