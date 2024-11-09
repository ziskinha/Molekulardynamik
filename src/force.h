#pragma once
#include <cmath>
#include <functional>
#include "Particle.h"
#include "utils/ArrayUtils.h"

namespace md::force {
	using ForceFunc = std::function<vec3(const Particle&, const Particle&)>;


	/**
	 * @brief Returns a function that calculates inverse-square force between two particles.
	 * @param pre_factor scaling factor for the magnitude of the force. can be used to implement newtons gravity or coulombs law
	 * @return The function that calculates the force vector acting on p1 due to p2.
	 */
	inline ForceFunc inverse_square(const double pre_factor = 1.0) {
		return [=](const Particle& p1, const Particle& p2) {
			const double dist = ArrayUtils::L2Norm(p1.position - p2.position);
			const double f_mag = p1.mass * p2.mass / pow(dist, 3);
			return pre_factor * f_mag * (p2.position - p1.position);
		};
	}


	/**
	 * @brief Returns a function that calculates the spring force using Hookes law between two particles.
	 * @param k spring constant
	 * @return The function that calculates the force vector acting on p1 due to p2.
	 */
	inline ForceFunc hookes_law(const double k = 0.1, const double rest_length = 0.0) {
		return [=](const Particle& p1, const Particle& p2) {
			double dist = ArrayUtils::L2Norm(p1.position - p2.position);
			double f_mag = k * (dist - rest_length) / dist;
			return f_mag * (p2.position - p1.position);
		};
	}


	/**
	 * @brief Returns a function that calculates the forces in a lennard jones potential between two particles.
	 * @param epsilon
	 * @param sigma
	 * @return The function that calculates the force vector acting on p1 due to p2.
	 */
	inline ForceFunc lennard_jones(const double epsilon = 1.0, const double sigma = 1.0) {
		return [=](const Particle& p1, const Particle& p2) {
			const double dist_squared = pow(p2.position[0] - p1.position[0], 2) +
				pow(p2.position[1] - p1.position[1], 2) +
				pow(p2.position[2] - p1.position[2], 2);
			return -24 * epsilon * ((pow(sigma, 6) / pow(dist_squared, 4)) - 2 * pow(sigma, 12) / pow(dist_squared, 7))
				* (p2.position - p1.position);
		};
	}
}
