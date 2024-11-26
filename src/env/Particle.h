#pragma once

#include <string>
#include "Common.h"

namespace md::env {
    class ParticleGrid;

    struct Particle {
        enum State {
            ALIVE = 0x1,
            DEAD = 0x2,
            STATIONARY = 0x4,
        };

        Particle(size_t id, ParticleGrid& grid, const vec3& position, const vec3& velocity, double mass, int type);
        Particle(const Particle& other);

        void reset_force();
        void update_grid();

        bool operator==(const Particle& other) const;
        [[nodiscard]] std::string to_string() const;

        vec3 position;
        vec3 velocity;
        vec3 force;
        vec3 old_force;
        int3 cell;

        double mass;
        int type;  // < 0: stationary; == PARTICLE_TYPE_DEAD: dead; else: alive
        const size_t id;
    private:
        ParticleGrid & grid;
    };


    // Enable bitwise operations for Particle::State
    constexpr Particle::State operator|(const Particle::State lhs, const Particle::State rhs) {
        using T = std::underlying_type_t<Particle::State>;
        return static_cast<Particle::State>(static_cast<T>(lhs) | static_cast<T>(rhs));
    }

    constexpr bool operator&(const Particle::State lhs, const Particle::State rhs) {
        using T = std::underlying_type_t<Particle::State>;
        return static_cast<T>(lhs) & static_cast<T>(rhs);
    }
}