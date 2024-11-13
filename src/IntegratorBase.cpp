
#include "IntegratorBase.h"
#include "io/Logger.h"

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
	IntegratorBase::IntegratorBase(ParticleContainer& particles, std::unique_ptr<io::OutputWriterBase> writer):
		particles(particles), writer(std::move(writer))
	{}

	void IntegratorBase::simulate(const double start_time, const double end_time, const double dt,
	                              const unsigned int write_freq, const bool benchmark)
	{
		int i = 0;
		const int total_steps = static_cast<int>((end_time - start_time) / dt);
        SPDLOG_INFO("Simulation started");

		if (benchmark) {
			auto start = std::chrono::high_resolution_clock::now();

			for (double t = start_time; t < end_time; t += dt) {
				simulation_step(dt);
			}
			auto end = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			SPDLOG_INFO("Execution time: {} milliseconds", duration);
			SPDLOG_INFO("Number of particles: {}", particles.size());
			SPDLOG_INFO("Number of steps: {}", total_steps);

		} else {
			for (double t = start_time; t < end_time; t += dt, i++ ) {
				simulation_step(dt);
				if (i % write_freq == 0) {
					if (writer != nullptr) {
						spdlog::debug("Plotting particles @ iteration {}, time {}", i, t);
						writer->plot_particles(particles, i);
					}
				}

				show_progress(i, total_steps);
			}
		}

        std::cout << "" << std::endl;
        SPDLOG_INFO("Simulation ended");
	}
}