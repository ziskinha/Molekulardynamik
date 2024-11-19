#pragma once
#include <cmath>
#include <limits>


#include "utils/ArrayUtils.h"


#define NO_FORCE_CUTOFF std::numeric_limits<double>::max()

namespace md::force {
    class Force {
    public:
        virtual ~Force() = default;
        explicit Force(const double cutoff): cutoff_radius(cutoff) {}
        virtual vec3 operator()(const Particle& p1, const Particle& p2) const = 0;
        const double cutoff_radius;
    };

    class InverseSquare final : public Force {
    public:
        /**
         * @brief InverseSquare force constructor.
         * @param pre_factor scaling factor for the magnitude of the force. can be used to implement newtons gravity or
         * coulombs law
         * @param cutoff Maximum distance for the force to be applied.
         */
        explicit InverseSquare(const double pre_factor = 1, const double cutoff = NO_FORCE_CUTOFF)
            : Force(cutoff), pre_factor(pre_factor) {}

        vec3 operator()(const Particle& p1, const Particle& p2) const override {
            const double dist = ArrayUtils::L2Norm(p1.position - p2.position);

            if (dist > cutoff_radius || dist == 0.0) {
                return vec3{0.0, 0.0, 0.0};
            }

            const double f_mag = p1.mass * p2.mass / pow(dist, 3);
            return -pre_factor * f_mag * (p2.position - p1.position);
        }
    private:
        const double pre_factor;
    };



    class NoForce final : public Force {
    public:
        explicit NoForce(): Force(NO_FORCE_CUTOFF) {}
        vec3 operator()(const Particle&, const Particle&) const override {
            return vec3{0.0, 0.0, 0.0};
        }
    };


    class HookesLaw final : public Force {
    public:
        /**
         * @brief Hooke's Law force constructor.
         * @param k Spring constant.
         * @param rest_length Rest length of the spring.
         */
        explicit HookesLaw(const double k = 0.1, const double rest_length = 0.0)
            : Force(NO_FORCE_CUTOFF), k(k), rest_length(rest_length) {}

        vec3 operator()(const Particle& p1, const Particle& p2) const override {
            const double dist = ArrayUtils::L2Norm(p1.position - p2.position);

            if (dist == 0.0) {
                return vec3{0.0, 0.0, 0.0};
            }

            const double f_mag = k * (dist - rest_length) / dist;
            return -f_mag * (p2.position - p1.position);
        }

    private:
        const double k;           // Spring constant
        const double rest_length; // Rest length of the spring
    };

    class LennardJones final : public Force {
    public:
        /**
         * @brief Lennard-Jones force constructor.
         * @param epsilon Depth of the potential well.
         * @param sigma  Approximate radius of repulsive component
         * @param cutoff Maximum distance for the force to be applied.
         */
        explicit LennardJones(const double epsilon = 1.0, const double sigma = 1.0, const double cutoff = NO_FORCE_CUTOFF)
            : Force(cutoff), epsilon(epsilon), sigma(sigma) {}

        vec3 operator()(const Particle& p1, const Particle& p2) const override {
            const vec3 diff = p2.position - p1.position;
            const double dist_squared = diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2];

            if (dist_squared > cutoff_radius * cutoff_radius || dist_squared == 0.0) {
                return vec3{0.0, 0.0, 0.0};
            }

            const double inv_dist_squared = 1.0 / dist_squared;
            const double inv_dist_6 = pow(inv_dist_squared, 3);
            const double inv_dist_12 = inv_dist_6 * inv_dist_6;

            const double scalar =
                24 * epsilon * inv_dist_squared * (2 * pow(sigma, 12) * inv_dist_12 - pow(sigma, 6) * inv_dist_6);

            return scalar * diff;
        }

    private:
        const double epsilon;
        const double sigma;
    };

}  // namespace md::force
