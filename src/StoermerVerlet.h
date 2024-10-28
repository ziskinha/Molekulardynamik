#pragma once


#include "IntegratorBase.h"
#include "force.hpp"


namespace md::Integrator {

	/**
	 * @brief Implements the calculation of Stoermer-Verlet.
	 */
	class StoermerVerlet final : public IntegratorBase {
	public:
		/**
		 * @brief Constructs a StoermerVerlet object with specified force calculation.
		 * @param particles contains the to-be-simulated particles
		 * @param force_func Defines the force function which is being used.
		 * @param writer writes the output to disk
		 */
		StoermerVerlet(ParticleContainer& particles, force::ForceFunc force_func, io::OutputWriter& writer);

	private:
		/**
		 * @brief Performs a single step of the Stoermer-Verlet.
		 * @param dt Δt The time increment for each simulation step.
		 */
		void simulation_step(double dt) override;
		force::ForceFunc force_func;
	};
}



