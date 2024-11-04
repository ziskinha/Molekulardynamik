
#include "IntegratorBase.h"

#include <iostream>


namespace md::Integrator {
	IntegratorBase::IntegratorBase(ParticleContainer& particles, std::unique_ptr<io::OutputWriter> writer):
		particles(particles), writer(std::move(writer))
	{}

	void IntegratorBase::simulate(const double start_time, const double end_time, const double dt,
	                              const unsigned int write_freq)
	{
		int i = 0;
		for (double t = start_time; t < end_time; t += dt, i++ ) {
			simulation_step(dt);
			if (i % write_freq == 0) {
				if (writer != nullptr) writer->plot_particles(particles, i);
			}

			// std::cout << "Iteration " << i << " finished." << std::endl;
		}
	}
}