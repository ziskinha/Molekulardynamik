#include "Force.h"

#include <ranges>

namespace md::env {

    Force::Force()
        : cutoff_radius(FORCE_CUTOFF_AUTO), force_func([](const vec3&, const Particle&, const Particle&) {
            return vec3{0, 0, 0};
        }) {}

    Force::Force(ForceFunc force_func, const double cutoff)
        : cutoff_radius(cutoff), force_func(std::move(force_func)) {}

    vec3 Force::operator()(const vec3& diff, const Particle& p1, const Particle& p2) const {
        return force_func(diff, p1, p2);
    }

    double Force::cutoff() const {
        return cutoff_radius;
    }




    Force InverseSquareForce(const double pre_factor, double cutoff_radius) {
        if (cutoff_radius == FORCE_CUTOFF_AUTO) cutoff_radius = 10 * pre_factor;

        const Force::ForceFunc force_func = [=](const vec3& diff, const Particle& p1, const Particle& p2) {
            const double dist = ArrayUtils::L2Norm(diff);
            if (dist > cutoff_radius) {
                return vec3{0.0, 0.0, 0.0};
            }
            const double f_mag = p1.mass * p2.mass / std::pow(dist, 3);
            return -pre_factor * f_mag * diff;
        };

        return Force(force_func, cutoff_radius);
    }


    Force LennardJonesForce(const double epsilon, const double sigma, double cutoff_radius) {
        if (cutoff_radius == FORCE_CUTOFF_AUTO) cutoff_radius = 3 * sigma;

        const Force::ForceFunc force_func = [=](const vec3& diff, const Particle&, const Particle&) {
            const double dist_squared = ArrayUtils::L2NormSquared(diff);
            if (dist_squared > cutoff_radius * cutoff_radius) return vec3{0.0, 0.0, 0.0};

            const double inv_r2 = 1.0 / dist_squared;
            const double sigma_r2 = (sigma * sigma) * inv_r2;
            const double sigma_r6 = sigma_r2 * sigma_r2 * sigma_r2;
            const double sigma_r12 = sigma_r6 * sigma_r6;
            const double scalar = 24.0 * epsilon * inv_r2 * (2.0 * sigma_r12 - sigma_r6);

            return scalar * diff;
        };

        return Force(force_func, cutoff_radius);
    }

    Force HarmonicForce(const double k, const double r0, double cutoff_radius) {
        if (cutoff_radius == FORCE_CUTOFF_AUTO) cutoff_radius = 2 * r0;

        const Force::ForceFunc force_func = [=](const vec3& diff, const Particle&, const Particle&) {
            const double dist = ArrayUtils::L2Norm(diff);
            return - k * (dist - r0) / dist * diff;
        };

        return Force(force_func, cutoff_radius);
    }




    ForceManager::ForceManager()
        : cutoff_radius(0) {}

    void ForceManager::init() {
        std::vector<int> types;
        for (const auto& key : std::views::keys(global_force_types)) {
           types.push_back(key);
        }

        for (size_t i = 0; i < types.size(); i++) {
            for (size_t j = 0; j < types.size(); j++) {
                auto force1 = global_force_types[static_cast<int>(i)];
                auto force2 = global_force_types[static_cast<int>(j)];

                global_forces[{types[i], types[j]}] = mix_forces(force1, force2);
                cutoff_radius = std::max(cutoff_radius, global_forces[{types[i], types[j]}].cutoff() );
            }
        }

        for (auto [particle_ids, force] : localized_force_types) {
            localized_forces[particle_ids] = mix_forces(force, force);
            cutoff_radius = std::max(cutoff_radius, localized_forces[particle_ids].cutoff() );
        }


        global_force_types.clear();
        localized_force_types.clear();
    }

    void ForceManager::add_force(const ForceType& force, const int particle_type) {
        global_force_types[particle_type] = force;
    }

    void ForceManager::add_force(const ForceType& force, const ParticleIDPair& particle_ids) {
        localized_force_types[particle_ids] = force;
    }

    vec3 ForceManager::evaluate(const vec3& diff, const Particle& p1, const Particle& p2) const {
        vec3 force = global_forces.at({p1.type, p2.type})(diff, p1, p2);

        if (!localized_forces.empty()) {
            if (const auto it = localized_forces.find({p1.id, p2.id}); it != localized_forces.end()) {
                force = force + it->second(diff, p1, p2);
            }
        }

        return force;
    }

    double ForceManager::cutoff() const {
        return cutoff_radius;
    }

    Force ForceManager::mix_forces(const ForceType& force1, const ForceType& force2) {
        // Check if both are Lennard-Jones
        if (const auto* lj1 = std::get_if<LennardJones>(&force1)) {
            if (const auto* lj2 = std::get_if<LennardJones>(&force2)) {
                const double eps = sqrt(lj1->epsilon * lj2->epsilon);
                const double sigma = (lj1->sigma + lj2->sigma) / 2;
                const double cutoff = std::max(lj1->cutoff, lj2->cutoff);
                return LennardJonesForce(eps, sigma, cutoff);
            }
        }
        // Check if both are inverse square
        if (const auto* is1 = std::get_if<InverseSquare>(&force1)) {
            if (const auto* is2 = std::get_if<InverseSquare>(&force2)) {
                const double G = sqrt(is1->pre_factor * is2->pre_factor);
                const double cutoff = std::max(is1->cutoff, is2->cutoff);
                return InverseSquareForce(G, cutoff);
            }
        }

        // Check if both are Harmonic
        if (const auto* h1 = std::get_if<Harmonic>(&force1)) {
            if (const auto* h2 = std::get_if<Harmonic>(&force2)) {
                const double r0 = (h1->r0 + h2->r0) / 2;
                const double k = (h1->k + h2->k) / 2;
                const double cutoff = std::max(h1->cutoff, h2->cutoff);
                return HarmonicForce(k, r0, cutoff);
            }
        }

        // Unsupported combination
        throw std::invalid_argument("Unsupported force type combination.");
    }
}
