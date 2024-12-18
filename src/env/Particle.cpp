#include "Particle.h"

#include <sstream>

#include "ParticleGrid.h"
#include "io/Logger/Logger.h"
#include "utils/ArrayUtils.h"

namespace md::env {
    /// -----------------------------------------
    /// \brief Particle Class Methods
    /// -----------------------------------------
    Particle::Particle(const size_t id, ParticleGrid& grid, const vec3& position, const vec3& velocity,
                       const double mass, const int type, const vec3& force)
        : position(position),
          old_position(),
          velocity(velocity),
          force(force),
          old_force(),
          cell(),
          mass(mass),
          type(type),
          id(id),
          state(ALIVE),
          grid(grid) {
        SPDLOG_TRACE("Particle generated! id: {}, position: {}", id, position);
    }

    void Particle::update_position(const SIMDVec3& dx) {
        old_position = position;
        position = old_position + dx;
    }

    void Particle::reset_force() {
        old_force = force;
        force = SIMDVec3();
    }

    void Particle::update_grid() {
        grid.update_cells(this, cell, grid.what_cell(position));
        cell = grid.what_cell(position);
    }

    std::string Particle::to_string() const {
        std::stringstream stream;

        stream << "Particle: X:" << position.toArray() << " v: " << velocity.toArray() << " f: " << force.toArray() << " old_f: " << old_force.toArray()
               << " type: " << type << " id: " << id;
        return stream.str();
    }

    // bool Particle::operator==(const Particle& other) const {
    //     return id == other.id;
    // }
    bool Particle::operator==(const Particle& other) const {
        return position.toArray() == other.position.toArray() && velocity.toArray() == other.velocity.toArray() && force.toArray() == other.force.toArray() &&
               old_force.toArray() == other.old_force.toArray() && mass == other.mass && type == other.type && state == other.state;
    }
}  // namespace md::env