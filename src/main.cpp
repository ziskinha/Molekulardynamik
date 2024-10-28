#include <iostream>


#include "Force.hpp"
#include "io/FileReader.h"
#include "io/OutputStrategy.h"

#include "StoermerVerlet.h"
#include "Particle.h"


int main(const int argc, char* argv[]) {
	std::cout << "Hello from MolSim for PSE!" << std::endl;
	if (argc != 5) {
		std::cout << "Erroneous programme call! " << std::endl;
		std::cout << "./MolSim filename end_time delta_t useVTK(0/1)" << std::endl;
		return 0;
	}

    //get arguments via command line
	constexpr double start_time = 0;
	double end_time = std::stod(argv[2]);
	double delta_t = std::stod(argv[3]);
    bool useVTK = std::stoi(argv[4]) != 0;

	md::ParticleContainer particles(md::io::FileReader::read_file(argv[1]));
	const auto writer = md::io::createWriter(useVTK, particles.size());

	md::Integrator::StoermerVerlet simulator(particles, md::Force::hookes_law(0.0071),*writer);
	simulator.simulate(start_time, end_time, delta_t, 200);

	std::cout << "output written. Terminating..." << std::endl;
}
