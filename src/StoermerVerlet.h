#pragma once


#include <functional>
#include "Integrator.h"
#include "utils/ArrayUtils.h"


namespace md::core {
	/**
	 * @brief Calculates the inverse-square force between two particles.
	 * @param p1
	 * @param p2
	 * @return The force vector acting on p1 due to p2.
	 */
	inline vec3 inverse_square_force(const Particle& p1, const Particle& p2) {
		double dist = ArrayUtils::L2Norm(p1.position - p2.position);
		double f_mag = p1.mass * p2.mass / pow(dist, 3);
		return f_mag * (p2.position - p1.position);
	}


	/**
	 * @brief Implements the calculation of Stoermer-Verlet.
	 */
	class StoermerVerlet final : public Integrator {
	public:
		using Force = std::function<vec3(const Particle&, const Particle&)>;

		/**
		 * @brief Constructs a StoermerVerlet object with specified force calculation.
		 * @param particles contains the to-be-simulated particles
		 * @param force_func Defines the force function which is being used.
		 * @param writer writes the output to disk
		 */
		StoermerVerlet(ParticleContainer& particles, Force force_func, io::OutputWriter& writer);

	private:
		/**
		 * @brief Performs a single step of the Stoermer-Verlet.
		 * @param dt Δt The time increment for each simulation step.
		 */
		void simulation_step(double dt) override;
		Force force_func;
	};
}



