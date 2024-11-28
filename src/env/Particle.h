#pragma once

#include <string>
#include "Common.h"

namespace md::env {
    class ParticleGrid;

    struct Particle {
        enum State {
            ALIVE = 0x1,
            DEAD = 0x2,
        };

        Particle(size_t id, ParticleGrid& grid, const vec3& position, const vec3& velocity, double mass, int type);
        Particle(const Particle& other);

        void update_position(const vec3 & dx);
        void reset_force();
        void update_grid();

        bool operator==(const Particle& other) const;
        [[nodiscard]] std::string to_string() const;

        vec3 position;
        vec3 old_position;
        vec3 velocity;
        vec3 force;
        vec3 old_force;
        int3 cell;

        double mass;
        int type;
        const size_t id;
        State state;
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