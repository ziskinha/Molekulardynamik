#pragma once

#include <array>
#include <vector>

#include "Particle.h"

/**
 * @brief Container class for managing a collection of Particle objects.
 */
class ParticleContainer {
public:
    ParticleContainer() = delete;

    /**
     * @brief Constructs a ParticleContainer with a specified collection of Particles.
     * @param particles The vector of Particle objects to initialize the container.
     */
    explicit ParticleContainer(const std::vector<Particle> & particles);


    using iterator = std::vector<Particle>::iterator;
    using const_iterator = std::vector<Particle>::const_iterator;

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

    Particle& operator[](size_t index) {
        return particles[index];
    }

    const Particle& operator[](size_t index) const {
        return particles[index];
    }

    // making class non-copyable to avoid accidentally copying all the data
	// may be changed in the future but currently there is no need to be able to copy it
    ParticleContainer(const ParticleContainer&) = delete;
    ParticleContainer& operator=(const ParticleContainer&) = delete;
private:
    std::vector<Particle> particles = {};
};



