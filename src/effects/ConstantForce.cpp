//
// Created by jdyma on 29/12/2024.
//

#include "ConstantForce.h"

#include "env/Environment.h"


namespace md::env {

    ConstantForce::ConstantForce(const vec3& direction, const double strength,
                                 ParticleMarker marker, const double start_time, const double end_time, const bool const_acceleration)
        : marker(std::move(marker)),
          direction(direction),
          strength(strength),
          const_acceleration(const_acceleration),
          start_time(start_time),
          end_time(end_time)
    {}

    void ConstantForce::mark_particles(const Environment& env) {
        for (const auto& p : env.particles()) {
            if (marker(p)) {
                marked.push_back(p.id);
            }
        }
    }

    void ConstantForce::apply_force(Particle& particle, const double t) const {
        if (t >= start_time && t <= end_time) {
            particle.force = particle.force + strength / (const_acceleration ? particle.mass : 1.0) * direction;
        }
    }

    const std::vector<size_t>& ConstantForce::marked_particles() {
        return marked;
    }

}

