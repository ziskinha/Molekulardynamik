#pragma once

#include "io/IOStrategy.h"
#include "Particle.h"

namespace md::Integrator {
	/**
	 * @brief Manages the main simulation loop.
	 */
	class IntegratorBase {
	public:
		/**
		 * @brief Constructs a SimulationAlgorithm object with a reference to a ParticleContainer and OutputWriter.
		 * @param particles
		 * @param writer
		 */
		IntegratorBase(ParticleContainer & particles, std::unique_ptr<io::OutputWriterBase> writer);
		virtual ~IntegratorBase() = default;

		/**
		 * @brief Runs the simulation over a given time interval with specified step size and output frequency.
		 * @param start_time
		 * @param end_time
		 * @param dt Δt The time increment for each simulation step.
		 * @param write_freq The frequency with which the data is written to output.
		 * @param benchmark
		 */
		 void simulate(double start_time, double end_time, double dt, unsigned int write_freq, bool benchmark = false);

	protected:
		/**
		 * @brief Abstract method for performing a single simulation step.
		 * @param dt Δt The time increment for each simulation step.
		 */
		virtual void simulation_step(double dt) = 0;
		ParticleContainer & particles;
	private:
		std::unique_ptr<io::OutputWriterBase> writer;
	};
}


