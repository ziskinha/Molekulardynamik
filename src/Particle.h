/*
 * Particle.h
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "utils/MaxwellBoltzmannDistribution.h"

namespace md {
    using vec3 = std::array<double, 3>;
}

namespace md {
    /**
     * @brief Struct representing a particle.
     */
    struct Particle {
        /**
         * @brief Constructor that creates a particle with a specified type.
         * @param type
         */
        explicit Particle(int type = 0);

        /**
         * @brief Constructor that creates a particle with position, velocity, mass and type.
         * @param position
         * @param velocity
         * @param mass
         * @param type (default value is 0).
         */
        Particle(const vec3& position, const vec3& velocity, double mass, int type = 0);

        /**
         * @brief Copy constructor that creates a new particle from an existing one.
         * @param other
         */
        Particle(const Particle& other);
        ~Particle();

        [[nodiscard]] std::string to_string() const;
        bool operator==(const Particle& other) const;

        vec3 position;
        vec3 velocity;
        vec3 force;
        vec3 old_force;
        double mass;
        int type;
    };

    std::ostream& operator<<(std::ostream& stream, Particle& p);

    /**
     * @brief Container class for managing a collection of Particle objects.
     */
    class ParticleContainer {
        using iterator = std::vector<Particle>::iterator;
        using const_iterator = std::vector<Particle>::const_iterator;

       public:
        /**
         * @brief Create an empty particle container
         */
        ParticleContainer() = default;

        /**
         * @brief Constructs a ParticleContainer with a specified collection of Particles.
         * @param particles The vector of Particle objects to initialize the container.
         */
        explicit ParticleContainer(const std::vector<Particle>& particles);
        explicit ParticleContainer(std::vector<Particle>&& particles);

        /**
         * @brief add a set of equidistant particles in form of a cuboid
         * @param origin coordinate of the lower left corner
         * @param init_v initial velocity. is applied to all particles
         * @param num_particles number of particles along the x,y,z axis
         * @param thermal_v mean-value of the velocity of the Brownian Motion
         * @param width distance between each particle
         * @param mass mass of all the particles
         * @param dimension either 2 or 3. controls whether the brownian motion is in 2d or 3d.
         * @param type type of the particle (optional)
         */
        void add_cuboid(const vec3& origin, const vec3& init_v, const std::array<uint32_t, 3>& num_particles,
                        double thermal_v, double width, double mass, uint8_t dimension, int type = 0);

        /**
         * @brief adds a vector of particles to the particle container
         * @param particles
         */
        void add_particles(const std::vector<Particle>& particles);

        /**
         * @brief Returns an iterator pointing to the beginning of the container.
         * @return An iterator to the first Particle in the container.
         */
        iterator begin() { return particles.begin(); }

        /**
         * @brief Returns an iterator pointing to the end of the container.
         * @return An iterator to the end of the container.
         */
        iterator end() { return particles.end(); }

        [[nodiscard]] const_iterator begin() const { return particles.begin(); }
        [[nodiscard]] const_iterator end() const { return particles.end(); }
        [[nodiscard]] const_iterator cbegin() const { return particles.cbegin(); }
        [[nodiscard]] const_iterator cend() const { return particles.cend(); }

        /**
         * @brief Returns number of particles in the container.
         * @return The size of the container.
         */
        [[nodiscard]] size_t size() const;

        Particle& operator[](size_t index);
        const Particle& operator[](size_t index) const;

        ParticleContainer operator+(const ParticleContainer& other) const;

        // making class non-copyable to avoid accidentally copying all the data
        // may be changed in the future but currently there is no need to be able to copy it
        // ParticleContainer(const ParticleContainer&) = delete;
        // ParticleContainer& operator=(const ParticleContainer&) = delete;

       protected:
        std::vector<Particle> particles = {};
    };
}  // namespace md
