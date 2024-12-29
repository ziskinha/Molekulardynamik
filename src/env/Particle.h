#pragma once

#include <string>

#include "Common.h"

/**
 * @brief Contains classes and structures for managing the environment of the simulation.
 */
namespace md::env {
    /**
     * @brief A class representing the particle grid.
     */
    class ParticleGrid;

    /**
     * @brief Structure representing a particle.
     */
    struct Particle {
        using Type = int;
        using ID = size_t;

        /**
         * @brief Enumeration of the possible states of a particle.
         */
        enum State {
            ALIVE = 0x1,
            DEAD = 0x2,
            STATIONARY = 0x4
        };

        /**
         * @brief Constructs a new Particle object with the given parameters.
         * @param id The id for the particle.
         * @param grid The ParticleGrid object that the particle belongs to.
         * @param position The initial position of the particle.
         * @param velocity The initial velocity of the particle.
         * @param mass The initial mass of the particle.
         * @param force The initial force of the particle.
         * @param type The type of the particle (default: {0, 0, 0}.
         * @param state
         */
        Particle(size_t id, ParticleGrid& grid, const vec3& position, const vec3& velocity, double mass, int type,
            State state = ALIVE, const vec3& force  = {0, 0, 0});

        /**
         * @brief Copy constructor for Particle.
         * @param other The particle to copy from.
         */
        Particle(const Particle& other);

        /**
         * @brief Updates the position of the particle.
         * @param dx
         */
        void update_position(const vec3 & dx);

        /**
         * @brief Resets the force to zero.
         */
        void reset_force();

        /**
         * @brief Updates the particle's grid cell.
         */
        void update_grid();

        /**
         * @brief Compares the particle to another regarding equality.
         * @param other The Particle object to compare with.
         * @return "true" if the particles are equal, "false" otherwise.
         */
        bool operator==(const Particle& other) const;

        /**
         * @brief Creates a string representation of the particle's properties.
         * @return A string of the particle's properties.
         */
        [[nodiscard]] std::string to_string() const;

        vec3 position;      ///< The current position of the particle.
        vec3 old_position;  ///< The previous position of the particle. Useful for applying boundary conditions
        vec3 velocity;      ///< The current velocity of the particle.
        vec3 force;         ///< The current force acting on the particle.
        vec3 old_force;     ///< The previous force acting on the particle.
        int3 cell;          ///< The current cell of the particle.

        double mass;      ///< The mass of the particle.
        Type type;         ///< The type of the particle.
        const ID id;  ///< The id of the particle.
        State state;      ///< The state of the particle.

       private:
        ParticleGrid& grid;  ///< The grid to which the particle belongs to.
    };

    /**
     * @brief Enables bitwise OR operation for Particle::State.
     * @param lhs
     * @param rhs
     * @return The result of the bitwise OR operation between lhs and rhs.
     */
    constexpr Particle::State operator|(const Particle::State lhs, const Particle::State rhs) {
        using T = std::underlying_type_t<Particle::State>;
        return static_cast<Particle::State>(static_cast<T>(lhs) | static_cast<T>(rhs));
    }

    /**
     * @brief Enables bitwise AND operation for Particle::State.
     * @param lhs
     * @param rhs
     * @return "true" if bitwise AND operation between lhs and rhs is non-zero, "false" otherwise.
     */
    constexpr bool operator&(const Particle::State lhs, const Particle::State rhs) {
        using T = std::underlying_type_t<Particle::State>;
        return static_cast<T>(lhs) & static_cast<T>(rhs);
    }
}  // namespace md::env
