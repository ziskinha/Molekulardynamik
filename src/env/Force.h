#pragma once
#include <cmath>
#include <functional>
#include <limits>
#include <utility>

#include "utils/ArrayUtils.h"
#include "Particle.h"

#define NO_FORCE_CUTOFF std::numeric_limits<double>::max()
#define FORCE_CUTOFF_AUTO std::numeric_limits<double>::min()

/**
 * @brief Contains classes and structures for managing the environment of the simulation.
 */
namespace md::env {
    using ForceFunc = std::function<vec3(const Particle&, const Particle&)>;

    /**
     * @brief Structure representing a force function with an optional cutoff distance.
     */
    struct Force {
        /**
         * @brief Constructs a Force object with a specified force function and cutoff.
         * @param force_func
         * @param cutoff
         */
        explicit Force(ForceFunc  force_func = {}, const double cutoff = NO_FORCE_CUTOFF)
            : cutoff_radius(cutoff), force_func(std::move(force_func)) {}

        /**
         * @brief Calculates the force between two particles.
         * @param p1 The first particle.
         * @param p2 The second particle.
         * @return The force between the two particles.
         */
        vec3 operator()(const Particle& p1, const Particle& p2) const { return force_func(p1, p2); }

        /**
         * @brief Retrieves the cutoff radius.
         * @return The cutoff radius.
         */
        [[nodiscard]] double cutoff() const { return cutoff_radius; }

       private:
        double cutoff_radius;
        ForceFunc force_func{};
    };

    /**
     * @brief Returns a Force object that represents no interaction between particles.
     * @param cutoff The cutoff distance.
     * @return A Force object representing no interaction.
     */
    inline Force NoForce(const double cutoff = 0) {
        return Force([](const Particle&, const Particle&) { return vec3{0, 0, 0}; }, cutoff);
    }

    /**
     * @brief Returns a Force object that represents the inverse-square force between two particles.
     * @param pre_factor scaling factor for the magnitude of the force. can be used to implement newtons gravity or
     * coulombs law.
     * @param cutoff_radius
     * @return The function that calculates the force vector acting on p1 due to p2.
     */
    inline Force InverseSquare(const double pre_factor = 1.0, double cutoff_radius = FORCE_CUTOFF_AUTO) {
        if (cutoff_radius == FORCE_CUTOFF_AUTO) cutoff_radius = 10 * pre_factor;

        const ForceFunc force_func = [=](const Particle& p1, const Particle& p2) {
            const double dist = ArrayUtils::L2Norm(p1.position - p2.position);
            if (dist > cutoff_radius) {
                return vec3{0.0, 0.0, 0.0};
            }
            const double f_mag = p1.mass * p2.mass / pow(dist, 3);
            return -pre_factor * f_mag * (p2.position - p1.position);
        };

        return Force(force_func, cutoff_radius);
    }

    /**
     * @brief Returns a function that calculates the spring force using Hookes law between two particles.
     * @param k spring constant
     * @param rest_length The rest length (default: 0.0).
     * @return The function that calculates the force vector acting on p1 due to p2.
     */
    inline Force HookesLaw(const double k = 0.1, const double rest_length = 0.0) {
        const ForceFunc force_func = [=](const Particle& p1, const Particle& p2) {
            const double dist = ArrayUtils::L2Norm(p1.position - p2.position);
            const double f_mag = k * (dist - rest_length) / dist;
            return -f_mag * (p2.position - p1.position);
        };

        return Force(force_func, NO_FORCE_CUTOFF);
    }

    /**
     * @brief Returns a function that calculates the forces in a lennard jones potential between two particles.
     * @param epsilon
     * @param sigma
     * @param cutoff_radius
     * @return The function that calculates the force vector acting on p1 due to p2.
     */
    inline Force LennardJones(const double epsilon = 1.0, const double sigma = 1.0,
                              double cutoff_radius = FORCE_CUTOFF_AUTO) {
        if (cutoff_radius == FORCE_CUTOFF_AUTO) cutoff_radius = 3 * sigma;

        const ForceFunc force_func = [=](const Particle& p1, const Particle& p2) {
            const double dist_squared = pow(p2.position[0] - p1.position[0], 2) +
                                        pow(p2.position[1] - p1.position[1], 2) +
                                        pow(p2.position[2] - p1.position[2], 2);

            if (dist_squared > cutoff_radius * cutoff_radius) {
                return vec3{0.0, 0.0, 0.0};
            }

            const double inv_dist_squared = 1.0 / dist_squared;
            const double inv_dist_6 = pow(inv_dist_squared, 3);
            const double inv_dist_12 = inv_dist_6 * inv_dist_6;

            const double scalar =
                24 * epsilon * inv_dist_squared * (2 * pow(sigma, 12) * inv_dist_12 - pow(sigma, 6) * inv_dist_6);

            return scalar * (p2.position - p1.position);
        };

        return Force(force_func, cutoff_radius);
    }
}  // namespace md::env
