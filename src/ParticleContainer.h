#pragma once

#include <array>
#include <vector>

#include "Particle.h"


class ParticleContainer {
public:
    ParticleContainer() = delete;
    explicit ParticleContainer(const std::vector<Particle> & particles);


    using iterator = std::vector<Particle>::iterator;
    using const_iterator = std::vector<Particle>::const_iterator;

    iterator begin() { return particles.begin(); }
    iterator end() { return particles.end(); }
    [[nodiscard]] const_iterator begin() const { return particles.begin(); }
    [[nodiscard]] const_iterator end() const { return particles.end(); }
    [[nodiscard]] const_iterator cbegin() const { return particles.cbegin(); }
    [[nodiscard]] const_iterator cend() const { return particles.cend(); }

    [[nodiscard]] size_t size() const;

    // making class non copyable to avoid accidentally copying all the data
    ParticleContainer(const ParticleContainer&) = delete;
    ParticleContainer& operator=(const ParticleContainer&) = delete;
private:
    std::vector<Particle> particles = {};
};



