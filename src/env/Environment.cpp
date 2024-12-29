#include "Environment.h"

#include <ranges>
#include <cmath>

#include "io/Logger/Logger.h"
#include "utils/ArrayUtils.h"
#include "utils/MaxwellBoltzmannDistribution.h"

#define WARN_IF_INIT(msg)                                                                                     \
    do {                                                                                                      \
        if (initialized) {                                                                                    \
            SPDLOG_ERROR("You are trying to {} in an initialized environment.", msg); \
            exit(-1);                                                                                           \
        }                                                                                                     \
    } while (0)

namespace md::env {
    /// -----------------------------------------
    /// \brief CreateInfo Constructors
    /// -----------------------------------------
    ParticleCreateInfo::ParticleCreateInfo(const vec3& position, const vec3& velocity, const double mass,
                                           const int type, Particle::State state)
        : position(position), velocity(velocity), mass(mass), type(type), state(state) {}

    CuboidCreateInfo::CuboidCreateInfo(const vec3& origin, const vec3& initial_v, const uint3& num_particles,
                                       const double width, const double mass, const double thermal_v, const int type,
                                       const Dimension dimension, const Particle::State state)
        : origin(origin),
          initial_v(initial_v),
          num_particles(num_particles),
          thermal_v(thermal_v),
          width(width),
          mass(mass),
          type(type),
          dimension(dimension),
          state(state)
    {}

    SphereCreateInfo::SphereCreateInfo(const vec3& origin, const vec3& initial_v, const int radius, const double width,
    const double mass, const double thermal_v, const int type, const Dimension dimension, const Particle::State state) :
    origin(origin),
      initial_v(initial_v),
      thermal_v(thermal_v),
      radius(radius),
      width(width),
      mass(mass),
      type(type),
      dimension(dimension),
      state(state)
    {}

    /// -----------------------------------------
    /// Environment Class Methods
    /// -----------------------------------------
    Environment::Environment()
        : dimension(Dimension::INFER), grid_constant(GRID_CONSTANT_AUTO), initialized(false), g_grav(0), num_stat_particles(0) {}

    /// -----------------------------------------
    ///  Methods for environment setup
    /// -----------------------------------------
    void Environment::set_force(const ForceType& force, const int particle_type) {
        WARN_IF_INIT("set the force");
        forces.add_force(force, particle_type);
    }

    void Environment::set_grid_constant(const double g) {
        WARN_IF_INIT("set the grid_constant");
        this->grid_constant = g;
    }

    void Environment::set_boundary(const Boundary& boundary) {
        WARN_IF_INIT("set the boundary");
        this->boundary = boundary;
    }

    void Environment::set_dimension(const Dimension dim) {
        WARN_IF_INIT("set the dimension");
        dimension = dim;
    }

    void Environment::set_gravity_constant(const double g) {
        g_grav = g;
    }

    vec3 Environment::gravity_force(const Particle& particle) const {
        return vec3{0, particle.mass * g_grav, 0};
    }

    vec3 Environment::average_velocity() {
        vec3 v = {};
        for (const auto& p : particle_storage) {
            v = v + p.velocity;
        }
        return 1 / size(Particle::ALIVE) * v;
    }

    void Environment::scale_thermal_velocity(double scalar, vec3 mean_v) {
        for (auto& particle : particles(GridCell::INSIDE, Particle::ALIVE)) {
            // combined with thermostat we perform the subtraction twice -> maybe rewrite code to only do this once?
            particle.velocity = scalar * (particle.velocity - mean_v) + mean_v;
        }
    }

    size_t Environment::add_particle(const vec3& position, const vec3& velocity, double mass, int type,
                                   const Particle::State state, const vec3& force) {
        WARN_IF_INIT("add particles");

        size_t id = particle_storage.size();
        particle_storage.emplace_back(id, grid, position, velocity, mass, type, state, force);
        if (state == Particle::STATIONARY) num_stat_particles++;
        SPDLOG_TRACE("Particle added to env. Position: [{}, {}, {}], Velocity: [{}, {}, {}], Mass: {}, Type: {}",
                     position[0], position[1], position[2], velocity[0], velocity[1], velocity[2], mass, type);
        return id;
    }

    void Environment::add_particles(const std::vector<ParticleCreateInfo>& particles) {
        for (auto& x : particles) {
            add_particle(x.position, x.velocity, x.mass, x.type, x.state);
        }
    }

    void Environment::add_cuboid(const CuboidCreateInfo& cuboid) {
        add_cuboid(cuboid.origin, cuboid.initial_v, cuboid.num_particles, cuboid.width, cuboid.mass,
        cuboid.thermal_v, cuboid.type, cuboid.dimension, cuboid.state);
    }

    void Environment::add_cuboid(const vec3& origin, const vec3& velocity, const uint3& num_particles, const double width,
        const double mass, const double thermal_v, const int type, const Dimension dimension, const Particle::State state) {
        WARN_IF_INIT("add particles (cuboids)");
        if (num_particles[0] == 0 || num_particles[1] == 0 || num_particles[2] == 0) {
            SPDLOG_ERROR("num_particles contains 0 particles in at least one direction");
            exit(3000);
        }

        auto dim = static_cast<uint8_t>(dimension);
        if (dimension == Dimension::INFER) {
            dim = num_particles[2] == 1 ? 2 : 3;
        }

        particle_storage.reserve(particle_storage.size() + num_particles[0] * num_particles[1] * num_particles[2]);

        for (unsigned int x = 0; x < num_particles[0]; ++x) {
            for (unsigned int y = 0; y < num_particles[1]; ++y) {
                for (unsigned int z = 0; z < num_particles[2]; ++z) {
                    vec3 pos = origin + vec3({x * width, y * width, z * width});
                    vec3 vel = velocity + maxwellBoltzmannDistributedVelocity(thermal_v, dim);
                    add_particle(pos, vel, mass, type, state);
                }
            }
        }
    }

    void Environment::add_sphere(const vec3& origin, const vec3& velocity, const int radius, const double width, const double mass,
        const double thermal_v, const int type, Dimension dimension, const Particle::State state) {
        WARN_IF_INIT("add particles (sphere)");
        if (dimension == Dimension::INFER) {
            SPDLOG_ERROR("cannot infer dimension of particle sphere");
            exit(3000);
        }
        int num_particles = 0;
        const auto dim = static_cast<uint8_t>(dimension);

        //calculate the number of particles to be added
        if (dim == 3) {
            num_particles = static_cast<int>(std::ceil((4.0 / 3.0) * M_PI * std::pow(radius / width, 3)));
        } else if (dim == 2) {
            num_particles = static_cast<int>(std::ceil(M_PI * std::pow(radius / width, 2)));
        }

        particle_storage.reserve(particle_storage.size() + num_particles);

        for (int x = -radius; x <= radius; ++x) {
            for (int y = -radius; y <= radius; ++y) {
                for (int z = -radius; z <= radius; ++z) {
                    if (dim == 2 && z != 0) continue;

                    const vec3 current_pos = {x * width, y * width, z * width};
                    const double distance_to_origin = ArrayUtils::L2Norm(current_pos);

                    if (distance_to_origin <= radius * width) {
                        vec3 pos = origin + current_pos;
                        vec3 vel = velocity + maxwellBoltzmannDistributedVelocity(thermal_v, dim);
                        add_particle(pos, vel, mass, type, state);
                    }
                }
            }
        }
    }

    void Environment::add_membrane(const vec3& origin, const vec3& velocity, const uint3& num_particles, const double width,
        const double mass, double k, const double cutoff, const int type) {

        if (num_particles[0] == 0 || num_particles[1] == 0 || num_particles[2] == 0) {
            SPDLOG_ERROR("num_particles contains 0 particles in at least one direction");
            exit(3000);
        }
        if (num_particles[2] != 1) {
            SPDLOG_ERROR("Membrane should be 2D");
            exit(3000);
        }

        particle_storage.reserve(particle_storage.size() + num_particles[0] * num_particles[1]);
        std::vector<size_t> particle_ids(num_particles[0] * num_particles[1]);

        auto index = [&](const unsigned int x, const unsigned int y) {
            return x + num_particles[0] * y;
        };

        // add particles
        for (unsigned int x = 0; x < num_particles[0]; ++x) {
            for (unsigned int y = 0; y < num_particles[1]; ++y) {
                vec3 pos = origin + vec3({x * width, y * width, 0});
                size_t particle_id = add_particle(pos, velocity, mass, type);
                particle_ids[index(x, y)] = particle_id;
            }
        }

        // add forces
        for (int x = 0; x < static_cast<int>(num_particles[0]); ++x) {
            for (int y = 0; y < static_cast<int>(num_particles[1]); ++y) {
                size_t id1 = particle_ids[index(x, y)];
                for (int dx = -1; dx <= 1; ++dx) {
                    for (int dy = -1; dy <= 1; ++dy) {
                        if (dx == 0 && dy == 0) continue;
                        const int nx = x + dx;
                        const int ny = y + dy;

                        if (nx >= 0 && nx < static_cast<int>(num_particles[0]) && ny >= 0 && ny < static_cast<int>(num_particles[1])) {
                            size_t id2 = particle_ids[index(nx, ny)];
                            const double r0 = ArrayUtils::L2Norm(particle_storage[id1].position - particle_storage[id2].position);
                            forces.add_force(Harmonic(k, r0, cutoff), {id1, id2});
                        }
                    }
                }
            }
        }
    }


    void Environment::add_sphere(const SphereCreateInfo& sphere) {
        add_sphere(sphere.origin, sphere.initial_v, sphere.radius, sphere.width, sphere.mass, sphere.thermal_v,
                   sphere.type, sphere.dimension, sphere.state);
    }

    void Environment::build() {
        if (initialized) {
            SPDLOG_ERROR("Environment is already initialized!");
            return;
        }

        SPDLOG_INFO("Start building the environment.");

        // check if boundary is ok
        if (boundary.extent[0] < 0 || boundary.extent[1] < 0 || boundary.extent[2] < 0) {
            SPDLOG_ERROR("Boundary extents must be non-negative.");
            throw std::invalid_argument("Boundary extents must be non-negative.");
        }
        for (int i = 0; i < 3; i++) {
            if (boundary.origin[i] == CENTER_BOUNDARY_ORIGIN) {
                boundary.origin[i] = -boundary.extent[i] / 2;
            }
        }
        for (Particle& particle : particle_storage) {
            const vec3 pos = particle.position - boundary.origin;
            if (pos[0] < 0 || pos[1] < 0 || pos[2] < 0 ||
                pos[0] > boundary.extent[0] || pos[1] > boundary.extent[1] || pos[2] > boundary.extent[2]) {
                SPDLOG_ERROR("Particle is being initialized outside of the boundary. Particle position: {}",
                             particle.position);
                throw std::invalid_argument("invalid particle position");
            }
        }
        if (boundary.requires_force_function()) {
            if (!boundary.has_force_function()) {
                SPDLOG_ERROR("Boundary requires a boundary force to be set but no such force has been given");
                throw std::invalid_argument("no boundary force");
            }
        }

        forces.init();
        // check if grid constant is ok
        if (grid_constant > 0 && grid_constant < forces.cutoff()) {
            SPDLOG_WARN(
                "Grid constant is smaller than force cutoff. Will default to use GRID_CONSTANT_AUTO."
                "Are you sure you setup the environment correctly?");
            grid_constant = forces.cutoff();
        }
        if (grid_constant == GRID_CONSTANT_AUTO) {
            if (boundary.extent[0] == MAX_EXTENT && boundary.extent[1] == MAX_EXTENT && boundary.extent[2] ==
                MAX_EXTENT) {
                grid_constant = MAX_EXTENT;
                SPDLOG_DEBUG("Using GRID_CONSTANT_AUTO. Grid constant set to MAX_EXTENT", grid_constant);
            } else {
                grid_constant = forces.cutoff();
                SPDLOG_DEBUG("Using GRID_CONSTANT_AUTO. Grid constant set to force cutoff: {}", grid_constant);
            }
        }
        if (grid_constant <= 0) {
            SPDLOG_ERROR(
                "Grid constant < 0. Grid constant should be chosen positive and at least the size of the force cutoff");
            throw std::invalid_argument("Grid constant is negative.");
        }

        // infer dimensionality
        if (dimension == Dimension::INFER) {
            dimension = Dimension::TWO;
            for (auto& p : particle_storage) {
                if (p.position[3] != 0 || p.velocity[3] != 0) {
                    dimension = Dimension::THREE;
                    break;
                }
            }
        }

        grid.build(boundary, grid_constant, particle_storage);
        initialized = true;
        SPDLOG_INFO("Environment successfully built.");
    }

    /// -----------------------------------------
    /// Methods for interacting with the environment
    /// -----------------------------------------
    double wrap_around_diff(const double x1, const double x2, const double n) {
        if (x2 > x1) return x2 - (x1 + n);
        return (x2 + n) - x1;
    }

    vec3 Environment::force(const Particle& p1, const Particle& p2, const CellPair& pair) const {
        if (p1.type == Particle::STATIONARY && p2.type == Particle::STATIONARY) return {};
        vec3 diff = p2.position - p1.position;

        // handle force wrap around
        if (pair.periodicity & CellPair::PERIODIC_X) {
            diff[0] = wrap_around_diff(p1.position[0], p2.position[0], boundary.extent[0]);
        }
        if (pair.periodicity & CellPair::PERIODIC_Y) {
            diff[1] = wrap_around_diff(p1.position[1], p2.position[1], boundary.extent[1]);
        }
        if (pair.periodicity & CellPair::PERIODIC_Z) {
            diff[2] = wrap_around_diff(p1.position[2], p2.position[2], boundary.extent[2]);
        }

        return forces.evaluate(diff, p1, p2);
    }

    size_t Environment::size(const Particle::State state) const {
        if (state == Particle::DEAD)
            return particle_storage.size() - grid.particle_count();
        if (state == Particle::STATIONARY)
            return num_stat_particles;
        if (state == Particle::ALIVE)
            return grid.particle_count() - num_stat_particles;
        if (state == (Particle::ALIVE | Particle::STATIONARY))
            return grid.particle_count();
        if (state == (Particle::DEAD | Particle::ALIVE | Particle::STATIONARY))
            return particle_storage.size();
        SPDLOG_ERROR("invalid particle state or unsupported state combination in Environment::size: {}", static_cast<int>(state));
        exit(3000);
    }

    const std::vector<CellPair>& Environment::linked_cells() {
        return grid.linked_cells();
    }

    void Environment::apply_boundary(Particle& particle) {
        const auto& current = grid.get_cell(particle.cell);
        const auto& previous = grid.get_cell(grid.what_cell(particle.old_position));
        boundary.apply_boundary(particle, current, previous);
    }

    double Environment::temperature(const vec3& avg_vel) const {
        double energy = 0;
        for (auto& particle : particles(GridCell::INSIDE, Particle::ALIVE)) {
            energy += particle.mass * ArrayUtils::L2NormSquared(particle.velocity - avg_vel);
        }
        return energy / static_cast<double>(dim() * size(Particle::ALIVE));
    }

    int Environment::dim() const {
        return static_cast<int>(dimension);
    }


    Particle& Environment::operator[](const size_t id) { return particle_storage[id]; }

    const Particle& Environment::operator[](const size_t id) const { return particle_storage[id]; }

    bool Environment::filter_particles(const Particle& particle, const Particle::State state,
                                       const GridCell::Type type) const {
        const bool state_ok = particle.state & state;
        const bool location_ok = grid.get_cell(particle).type & type;
        return state_ok && location_ok;
    }
} // namespace md::env
