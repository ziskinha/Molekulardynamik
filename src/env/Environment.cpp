
#include <ranges>

#include "Environment.h"
#include "io/Logger.h"
#include "utils/ArrayUtils.h"
#include "utils/MaxwellBoltzmannDistribution.h"

#define WARN_IF_INIT(msg) \
    do { \
        if (initialized) { \
        SPDLOG_WARN("You are trying to {} in an initialized environment. Request will be ignored",  msg);\
        return; \
        } \
    }while (0)



namespace md::env {

    /// -----------------------------------------
    /// \brief CreateInfo Constructors
    /// -----------------------------------------
    ParticleCreateInfo::ParticleCreateInfo(const vec3& position, const vec3& velocity, const double mass,
                                           const int type)
        : position(position), velocity(velocity), mass(mass), type(type) {}


    CuboidCreateInfo::CuboidCreateInfo(const vec3& origin, const vec3& initial_v, const uint3& num_particles,
                                       const double thermal_v,
                                       const double width, const double mass, const uint8_t dimension, const int type)
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
    Environment::Environment()
        : grid_constant(std::numeric_limits<double>::max()), force_cutoff(0),
          initialized(false) {}

    void Environment::set_force(const Force& force) {
        WARN_IF_INIT("set the force");
        this->force_func = force;
    }

    void Environment::set_force_cutoff(const double cutoff_radius) {
        force_cutoff = cutoff_radius;
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
        grid.build(boundary.extent, grid_constant, particle_storage, force_func.cutoff());
        initialized = true;
    }

    vec3 Environment::force(const Particle& p1, const Particle& p2) const {
        return force_func(p1, p2);
    }

    size_t Environment::size(Particle::State) const {
        return particle_storage.size();
    }

    Particle& Environment::operator[](const size_t id) {
        return particle_storage[id];
    }

    const Particle& Environment::operator[](const size_t id) const {
        return particle_storage[id];
    }

    bool Environment::filter_particles(const Particle& particle, const Particle::State state,
                                       const GridCell::Type type) const {
        const bool state_ok = (particle.type == PARTICLE_TYPE_DEAD && state & Particle::DEAD) ||
                              (particle.type < 0 && state & Particle::STATIONARY) ||
                              (particle.type >= 0 && particle.type != PARTICLE_TYPE_DEAD && state & Particle::ALIVE);
        const bool location_ok = grid.get_cell(particle).type & type;
        return state_ok && location_ok;
    }
} // namespace md
