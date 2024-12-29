#pragma once

#include <limits>
#include "env/Common.h"
#include "env/Particle.h"


namespace md::env {
    class Environment;
    using ParticleMarker = std::function<bool(const Particle&)>;
    ParticleMarker MarkAll();

    class ConstantForce {
    public:
        ConstantForce(const vec3 & direction, double strength, ParticleMarker marker = MarkAll(), double start_time = 0, double end_time = std::numeric_limits<double>::max(),  bool const_acceleration=false);

        void mark_particles(const Environment& env);

        void apply_force(Particle & particle, double t) const;

        const std::vector<size_t> & marked_particles();
    private:
        std::vector<size_t> marked;

        ParticleMarker marker;
        vec3 direction;
        double strength;
        bool const_acceleration;
        double start_time;
        double end_time;
    };

    inline ParticleMarker MarkAll() {
        return [](const Particle&) { return true;};
    }

    inline ParticleMarker MarkBox(const vec3 & x1, const vec3 & x2) {
        return [x1, x2] (const Particle& particle) {
            const vec3 & x = particle.position;
            return (x[0] >= x1[0] && x[0] <= x2[0]) && (x[1] >= x1[1] && x[1] <= x2[1]) && (x[2] >= x1[2] && x[2] <= x2[2]);
        };
    }

    inline ConstantForce Gravity(const double g, const int dim = 3) {
        return ConstantForce (dim == 2 ? vec3{0,1,0} : vec3{0,0,1}, g,MarkAll(), 0, std::numeric_limits<double>::max(), true );
    }
}



