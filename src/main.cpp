#include "FileReader.h"
#include "outputWriter/XYZWriter.h"
#include "outputWriter/VTKWriter.h"

#include <iostream>

#include "StoermerVerlet.h"
#include "ParticleContainer.h"


int main(const int argc, char* argv[]) {
	std::cout << "Hello from MolSim for PSE!" << std::endl;
	if (argc != 4) {
		std::cout << "Erroneous programme call! " << std::endl;
		std::cout << "./MolSim filename end_time delta_t" << std::endl;
		return 0;
	}

	constexpr double start_time = 0;
	double end_time = std::stod(argv[2]);
	double delta_t = std::stod(argv[3]);

	ParticleContainer particles(FileReader::read_file(argv[1]));
	outputWriter::VTKWriter writer("MD_vtk", particles.size());

	StoermerVerlet simulator(particles, writer);
	simulator.simulate(start_time, end_time, delta_t);

	std::cout << "output written. Terminating..." << std::endl;
	return 0;
}
