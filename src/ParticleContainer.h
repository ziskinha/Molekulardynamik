

#pragma once

#include <array>
#include <vector>

#include "Particle.h"


using vec3 = std::array<double, 3>;

class ParticleContainer {
public:
    ParticleContainer() = default;
    template <typename Container> explicit ParticleContainer(const Container& container);

    using iterator = std::vector<Particle>::iterator;
    using const_iterator = std::vector<Particle>::const_iterator;

    iterator begin() { return particles.begin(); }
    iterator end() { return particles.end(); }
    [[nodiscard]] const_iterator begin() const { return particles.begin(); }
    [[nodiscard]] const_iterator end() const { return particles.end(); }
    [[nodiscard]] const_iterator cbegin() const { return particles.cbegin(); }
    [[nodiscard]] const_iterator cend() const { return particles.cend(); }
private:
    std::vector<Particle> particles;
};



