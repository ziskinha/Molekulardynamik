
#include "Environment.h"

#include <ranges>

#include "io/Logger.h"
#include "utils/ArrayUtils.h"
#include "utils/MaxwellBoltzmannDistribution.h"

#define WARN_IF_INIT(msg)                                                                                    \
    do {                                                                                                     \
        if (initialized) {                                                                                   \
            SPDLOG_WARN("You are trying to {} in an initialized environment. Request will be ignored", msg); \
            return;                                                                                          \
        }                                                                                                    \
    } while (0)

namespace md::env {

    /// -----------------------------------------
    /// \brief CreateInfo Constructors
    /// -----------------------------------------
    ParticleCreateInfo::ParticleCreateInfo(const vec3& position, const vec3& velocity, const double mass,
                                           const int type)
        : position(position), velocity(velocity), mass(mass), type(type) {}

    CuboidCreateInfo::CuboidCreateInfo(const vec3& origin, const vec3& initial_v, const uint3& num_particles,
                                       const double thermal_v, const double width, const double mass,
                                       const uint8_t dimension, const int type)
        : origin(origin),
          initial_v(initial_v),
          num_particles(num_particles),
          thermal_v(thermal_v),
          width(width),
          mass(mass),
          dimension(dimension),
          type(type) {}

    /// -----------------------------------------
    /// \brief Environment Class Methods
    /// -----------------------------------------
    Environment::Environment() : force_func(NoForce()), grid_constant(GRID_CONSTANT_AUTO), initialized(false) {}

    /// -----------------------------------------
    /// \brief Methods for environment setup
    /// -----------------------------------------
    void Environment::set_force(const Force& force) {
        WARN_IF_INIT("set the force");
        this->force_func = force;
    }

    void Environment::set_grid_constant(const double g) {
        WARN_IF_INIT("set the grid_constant");
        this->grid_constant = g;
    }

    void Environment::set_boundary(const Boundary& boundary) {
        WARN_IF_INIT("set the boundary");
        this->boundary = boundary;
    }

    void Environment::add_particle(const vec3& position, const vec3& velocity, double mass, int type) {
        WARN_IF_INIT("add particles");
        particle_storage.emplace_back(particle_storage.size(), grid, position, velocity, mass, type);
    }

    void Environment::add_particles(const std::vector<ParticleCreateInfo>& particles) {
        for (auto& x : particles) {
            add_particle(x.position, x.velocity, x.mass, x.type);
        }
    }

    void Environment::add_cuboid(const vec3& origin, const vec3& initial_v, const uint3& num_particles,
                                 const double thermal_v, const double width, const double mass, uint8_t const dimension,
                                 const int type) {
        WARN_IF_INIT("add particles (cuboids)");

        particle_storage.reserve(particle_storage.size() + num_particles[0] + num_particles[1] + num_particles[2]);

        for (unsigned int x = 0; x < num_particles[0]; ++x) {
            for (unsigned int y = 0; y < num_particles[1]; ++y) {
                for (unsigned int z = 0; z < num_particles[2]; ++z) {
                    vec3 pos = origin + vec3({x * width, y * width, z * width});
                    vec3 vel = initial_v + maxwellBoltzmannDistributedVelocity(thermal_v, dimension);
                    add_particle(pos, vel, mass, type);
                }
            }
        }
    }

    void Environment::add_cuboid(const CuboidCreateInfo& cuboid) {
        add_cuboid(cuboid.origin, cuboid.initial_v, cuboid.num_particles, cuboid.thermal_v, cuboid.width, cuboid.mass,
                   cuboid.dimension, cuboid.type);
    }

    void Environment::build() {
        // TODO check parameters
        if (initialized) {
            SPDLOG_WARN("Environment is already initialized!");
            return;
        }
        // check if boundary is ok
        if (boundary.extent[0] < 0 || boundary.extent[1] < 0 || boundary.extent[2] < 0) {
            SPDLOG_ERROR("Boundary extents must be non-negative.");
            throw std::invalid_argument("Boundary extents must be non-negative.");
        }

        // check if grid constant is ok
        if (grid_constant > 0 && grid_constant <= force_func.cutoff()) {
            SPDLOG_WARN(
                "Grid constant is smaller than force cutoff. Will default to use GRID_CONSTANT_AUTO."
                "Are you sure you setup the environment correctly?");
            grid_constant = force_func.cutoff();
        }
        if (grid_constant < 0) {
            SPDLOG_ERROR("Grid constant is negative. Will default to use GRID_CONSTANT_AUTO.");
            throw std::invalid_argument("Grid constant is negative.");
        }
        if (grid_constant == GRID_CONSTANT_AUTO) {
            if (boundary.extent[0] == MAX_EXTENT && boundary.extent[1] == MAX_EXTENT &&
                boundary.extent[2] == MAX_EXTENT) {
                grid_constant = MAX_EXTENT;
            } else {
                grid_constant = force_func.cutoff();
                SPDLOG_DEBUG("Using GRID_CONSTANT_AUTO. Grid constant set to force cutoff: {}", grid_constant);
            }
        }

        for (int i = 0; i < 3; i++) {
            if (boundary.origin[i] == CENTER_BOUNDARY_ORIGIN) {
                boundary.extent[i] = -boundary.extent[i] / 2;
            }
        }

        grid.build(boundary.extent, grid_constant, particle_storage, boundary.origin);
        initialized = true;
    }

    /// -----------------------------------------
    /// \brief Methods for interacting with the environment
    /// -----------------------------------------
    vec3 Environment::force(const Particle& p1, const Particle& p2) const { return force_func(p1, p2); }

    size_t Environment::size(Particle::State) const {
        // todo: query number of particles in a given state
        return particle_storage.size();
    }

    const std::vector<GridCellPair> & Environment::linked_cells() {
        return grid.linked_cells();
    }

    // std::vector<GridCell> Environment::cells() {
    //     return grid.grid_cells();
    // }

    void Environment::apply_boundary(Particle& particle) {
        auto & current = grid.get_cell(particle.cell);
        auto & previous = grid.get_cell(grid.what_cell(particle.old_position));
        boundary.apply_boundary(particle, current, previous);
    }

    Particle& Environment::operator[](const size_t id) { return particle_storage[id]; }

    const Particle& Environment::operator[](const size_t id) const { return particle_storage[id]; }

    bool Environment::filter_particles(const Particle& particle, const Particle::State state,
                                       const GridCell::Type type) const {
        const bool state_ok = particle.state & state;
        const bool location_ok = grid.get_cell(particle).type & type;
        return state_ok && location_ok;
    }
}  // namespace md::env
