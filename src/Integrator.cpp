
#include "Integrator.h"

#include <iostream>


namespace md::core {
	Integrator::Integrator(ParticleContainer& particles, io::OutputWriter & writer):
		particles(particles), writer(writer)
	{}

	void Integrator::simulate(const double start_time, const double end_time, const double dt,
									   unsigned int write_freq)
	{
		int i = 0;
		for (double t = start_time; t < end_time; t += dt, i++ ) {
			simulation_step(dt);
			if (i % write_freq == 0) {
				writer.plot_particles(particles, i);
			}

			std::cout << "Iteration " << i << " finished." << std::endl;
		}
	}
}