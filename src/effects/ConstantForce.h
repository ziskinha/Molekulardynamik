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
        /**
         * @brief Constructor.
         * @param direction The direction of the force.
         * @param strength The strength of the force.
         * @param marker The function to select which particles are affected.
         * @param start_time Start time of the force application.
         * @param end_time End time of the force application.
         * @param const_acceleration Indicates if the force applies constant acceleration.
         */
        ConstantForce(const vec3 & direction, double strength, ParticleMarker marker = MarkAll(), double start_time = 0, double end_time = std::numeric_limits<double>::max(),  bool const_acceleration=false);

        /**
         * @brief Marks the particles that should be affected by the force.
         * @param env The environment of the particles.
         */
        void mark_particles(const Environment& env);
        /**
         * @brief Applies the force to a particle at a given time.
         * @param particle The particle to which the force is applied.
         * @param t The simulation time.
         */
        void apply_force(Particle & particle, double t) const;
        /**
         * @brief Retrieves the IDs of the marked particles.
         * @return A reference to a vector containing the IDs.
         */
        const std::vector<size_t> & marked_particles();
    private:
        std::vector<size_t> marked;  ///< List of particle IDs affected by the force.
        ParticleMarker marker;       ///< Function to select which particles are affected.
        vec3 direction;              ///< Direction of the force.
        double strength;             ///< Strength of the force.
        bool const_acceleration;     ///< Indicates if the force applies constant acceleration.
        double start_time;           ///< Start time of the force application.
        double end_time;             ///< End time of the force application.
    };

    /**
     * @brief A marker function that selects all particles.
     * @return A 'ParticleMaker' function that always returns true.
     */
    inline ParticleMarker MarkAll() {
        return [](const Particle&) { return true;};
    }

    /**
     * @brief A marker function that selects particles within a specific box region.
     * @param x1 The coordinates of the bottom-left-front corner of the box.
     * @param x2 The coordinates of the top-right-back corner of the box.
     * @return A 'ParticleMarker' function that returns 'true' if a particle's position lies within the bounds of the
     * box, 'false' otherwise.
     */
    inline ParticleMarker MarkBox(const vec3 & x1, const vec3 & x2) {
        return [x1, x2] (const Particle& particle) {
            const vec3 & x = particle.position;
            return (x[0] >= x1[0] && x[0] <= x2[0]) && (x[1] >= x1[1] && x[1] <= x2[1]) && (x[2] >= x1[2] && x[2] <= x2[2]);
        };
    }

    /**
     * @brief A factory function for creating gravitational force.
     * @param g The gravitational acceleration.
     * @param axis The direction of the gravitational force (default: {0, 0, 1}).
     * @return A 'ConstantForce' object representing the gravitational force.
     */
    inline ConstantForce Gravity(const double g, const vec3 & axis = vec3{0, 0, 1}) {
        return ConstantForce (axis, g,MarkAll(), 0, std::numeric_limits<double>::max(), true );
    }
} // namespace md::env



