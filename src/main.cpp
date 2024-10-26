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

	outputWriter::VTKWriter writer;
	ParticleContainer particles(FileReader::read_file(argv[1]));

	StoermerVerlet simulator(particles, writer);
	simulator.simulate(start_time, end_time, delta_t);

	std::cout << "output written. Terminating..." << std::endl;
	return 0;
}


// void plotParticles(int iteration) {
// 	//xyz output
// 	std::string out_name_xyz("MD_xyz");
// 	outputWriter::XYZWriter writer_XYZ;
// 	writer_XYZ.plotParticles(particles, out_name_xyz, iteration);
//
// 	//vtk output
// 	std::string out_name_vtk("MD_vtk");
// 	outputWriter::VTKWriter writer_VTK;
// 	writer_VTK.initializeOutput(int(particles.size()));
// 	// @TODO: Put loop into VTKWriter::plotParticle?
// 	for (auto& p : particles) {
// 		writer_VTK.plotParticle(p);
// 	}
// 	writer_VTK.writeFile(out_name_vtk, iteration);
// }
