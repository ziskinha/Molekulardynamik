#include "Particle.h"

#include <sstream>
#include "utils/ArrayUtils.h"
#include "io/Logger.h"
#include "ParticleGrid.h"


namespace md::env {
    /// -----------------------------------------
    /// \brief Particle Class Methods
    /// -----------------------------------------
    Particle::Particle(const size_t id, ParticleGrid& grid, const vec3& position, const vec3& velocity, const double mass, const int type)
        : position(position), velocity(velocity), force(), old_force(), cell(), mass(mass), type(type), id(id), state(ALIVE),
          grid(grid) {
        SPDLOG_TRACE("Particle generated!");
    }

    Particle::Particle(const Particle& other)
        : position(other.position),
          velocity(other.velocity),
          force(other.force),
          old_force(other.old_force),
          cell(other.cell),
          mass(other.mass),
          type(other.type),
          id(other.id),
          state(other.state),
          grid(other.grid) {
        SPDLOG_TRACE("Particle generated by copy!");
    }

    void Particle::reset_force() {
        old_force = force;
        force = {0, 0, 0};
    }

    void Particle::update_grid() {
        grid.update_cells(this, cell, grid.what_cell(position));
        cell = grid.what_cell(position);
    }

    std::string Particle::to_string() const {
        std::stringstream stream;
        using ::operator<<;
        stream << "Particle: X:" << position << " v: " << velocity << " f: " << force << " old_f: " << old_force
            << " type: " << type << " id: " << id;
        return stream.str();
    }

    bool Particle::operator==(const Particle& other) const {
        return position == other.position && velocity == other.velocity && force == other.force &&
               old_force == other.old_force && mass == other.mass && type == other.type && state == other.state;
    }
}