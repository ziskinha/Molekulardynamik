
#include "IntegratorBase.h"

#include <iostream>
#include <iomanip>
#include <chrono>

void show_progress(const int current, const int total) {
	constexpr int bar_width = 50;
	const float progress = static_cast<float>(current) / total;

	std::cout << "\r[";
	int pos = bar_width * progress;
	for (int i = 0; i < bar_width; ++i) {
		if (i < pos) std::cout << "=";
		else if (i == pos) std::cout << ">";
		else std::cout << " ";
	}
	std::cout << "] " << std::fixed << std::setprecision(2) << (progress * 100.0) << "%";
	std::cout.flush();
	if (current == total) {
		std::cout << std::endl;
	}
}

namespace md::Integrator {
	IntegratorBase::IntegratorBase(ParticleContainer& particles, std::unique_ptr<io::OutputWriter> writer):
		particles(particles), writer(std::move(writer))
	{}

	void IntegratorBase::simulate(const double start_time, const double end_time, const double dt,
	                              const unsigned int write_freq, const bool benchmark)
	{
		int i = 0;
		const int total_steps = static_cast<int>((end_time - start_time) / dt);

		if (benchmark) {
			auto start = std::chrono::high_resolution_clock::now();

			for (double t = start_time; t < end_time; t += dt) {
				simulation_step(dt);
			}
			auto end = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			std::cout << "Execution time: " << duration << " milliseconds" << std::endl;
			std::cout << "Number of particles:" << particles.size() << std::endl;
			std::cout << "Number of steps:" << total_steps << std::endl;

		} else {
			for (double t = start_time; t < end_time; t += dt, i++ ) {
				simulation_step(dt);
				if (i % write_freq == 0) {
					if (writer != nullptr) writer->plot_particles(particles, i);
				}

				show_progress(i, total_steps);
			}
		}
	}
}