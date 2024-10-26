
#include "SimulationAlgorithm.h"

#include <iostream>

// void plotParticles(int iteration, outputWriter::VTKWriter & writer) {
// 	//xyz output
// 	std::string out_name_xyz("MD_xyz");
// 	writer_XYZ.plotParticles(particles, out_name_xyz, iteration);
//
// 	//vtk output
// 	std::string out_name_vtk("MD_vtk");
// 	outputWriter::VTKWriter writer_VTK;
// 	writer_VTK.initializeOutput(int(particles.size()));
// 	// @TODO: Put loop into VTKWriter::plotParticle?
// 	for (auto &p : particles) {
// 		writer_VTK.plotParticle(p);
// 	}
// 	writer_VTK.writeFile(out_name_vtk, iteration);
// }



SimulationAlgorithm::SimulationAlgorithm(ParticleContainer& particles, outputWriter::VTKWriter & writer):
	particles(particles), writer(writer)
{}

void SimulationAlgorithm::simulate(const double start_time, const double end_time, const double dt,
                                   unsigned int write_freq)
{
	unsigned int i = 0;
	for (double t = start_time; t < end_time; t += dt, i++ ) {
		simulation_step(dt);
		if (i % 10 == 0) {
			// plotParticles(iteration, writer);
		}

		std::cout << "Iteration " << i << " finished." << std::endl;
	}
}
