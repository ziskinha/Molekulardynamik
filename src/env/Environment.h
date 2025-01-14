#pragma once

#include <array>
#include <ranges>
#include <vector>

#include "Boundary.h"
#include "Common.h"
#include "Force.h"
#include "Particle.h"
#include "ParticleGrid.h"

#define GRID_CONSTANT_AUTO 0

/**
 * @brief Contains classes and structures for managing the environment of the simulation.
 */
namespace md::env {

    /**
     * @brief Number of dimensions for the simulation.
     */
    enum class Dimension {
        TWO = 2,
        THREE = 3,
        INFER = -1
    };

    /**
     * @brief Structure containing information required to create a particle.
     */
    struct ParticleCreateInfo {
        /**
         * @brief Constructs a ParticleCreateInfo instance.
         * @param position Position vector of the particle.
         * @param velocity Velocity vector of the particle.
         * @param mass Mass of particle (default: 0).
         * @param type Type of particle (default: 0).
         * @param state
         */
        ParticleCreateInfo(const vec3& position, const vec3& velocity, double mass, int type = 0,  Particle::State state = Particle::ALIVE);
        vec3 position{};
        vec3 velocity{};
        double mass = 0;
        int type = 0;
        Particle::State state = Particle::ALIVE;
    };

    /**
     * @brief Structure containing information required to create a cuboid of particles.
     */
    struct CuboidCreateInfo {
        /**
         * @brief Constructs a CuboidCreateInfo instance.
         * @param origin Coordinates of the lower left front-side corner.
         * @param initial_v Initial velocity for all particles.
         * @param num_particles Number of particles along each dimension: N1 x N2 x N3.
         * @param thermal_v Thermal velocity of the particles.
         * @param width Distance between the particles.
         * @param mass Mass of each particle.
         * @param dimension Dimension of the cuboid.
         * @param type Type of each particle (default: 0).
         * @param state
         */
        CuboidCreateInfo(const vec3& origin, const vec3& initial_v, const uint3& num_particles, double width,
            double mass, double thermal_v = 0, int type = 0, Dimension dimension = Dimension::INFER,
            Particle::State state = Particle::ALIVE);
        vec3 origin;
        vec3 initial_v;
        uint3 num_particles;
        double thermal_v{};
        double width{};
        double mass{};
        int type = 0;
        Dimension dimension = Dimension::INFER;
        Particle::State state = Particle::ALIVE;
    };

    /**
     * @brief Structure containing information required to create a sphere of particles.
     */
    struct SphereCreateInfo {
        /**
         * @brief Constructs a SphereCreateInfo.
         * @param origin Coordinates of the center.
         * @param initial_v Initial velocity of all particles.
         * @param thermal_v Thermal velocity of the particles.
         * @param radius The radius in terms of the number of molecules along the radius.
         * @param width Distance between the particles.
         * @param mass The mass of the particles.
         * @param dimension Dimension of the sphere.
         * @param type The type of each particle (default: 0).
         * @param state
         */
        SphereCreateInfo(const vec3& origin, const vec3& initial_v, int radius, double width, double mass,
            double thermal_v = 0, int type = 0, Dimension dimension = Dimension::INFER,
            Particle::State state = Particle::ALIVE);
        vec3 origin;
        vec3 initial_v;
        double thermal_v;
        int radius;
        double width;
        double mass;
        int type;
        Dimension dimension = Dimension::INFER;
        Particle::State state = Particle::ALIVE;
    };


    /**
     * @brief Class representing the simulation environment, which manages particles, forces, boundaries, and the grid.
     */
    class Environment {
    public:
        /**
         * @brief Constructs an empty environment.
         */
        Environment();

        /**
         * @brief Initializes the simulation environment, setting up the grid and boundary conditions.
         * It checks the validity of the boundary extents and grid constant. If necessary, applies default values.
         */
        void build();


        /**
         * @brief Sets the grid constant, the desired side length of a cell.
         * @param g New grid constant.
         */
        void set_grid_constant(double g);
        /**
         * @brief Sets the force with which particles interact of a given type interact.
         * @param force The type force to be used.
         * @param particle_type
         */
        void set_force(const ForceType& force, int particle_type);
        /**
         * @brief Sets the boundary conditions for the environment.
         * @param boundary The boundary condition to be used.
         */
        void set_boundary(const Boundary& boundary);
        /**
        * @brief Sets the dimension of the environment.
        * @param dim
        */
        void set_dimension(Dimension dim);
        /**
         * @brief Adds a single particle to the environment.
         * @param position Position of the particle.
         * @param velocity Velocity of the particle.
         * @param mass Mass of the particle.
         * @param type Type of the particle.
         * @param state State of the particle (default: ALIVE).
         * @param force Initial force of the particle (default: {0, 0, 0}).
         */
        size_t add_particle(const vec3& position, const vec3& velocity, double mass, int type = 0, Particle::State state = Particle::ALIVE, const vec3& force = {});
        /**
         * @brief Adds multiple particles to the environment.
         * @param particles A ParticleCreateInfo vector describing the particles.
         */
        void add_particles(const std::vector<ParticleCreateInfo>& particles);
        /**
         * @brief Adds a cuboid to the environment.
         * @param cuboid A CuboidCreateInfo describing the cuboid.
         */
        void add_cuboid(const CuboidCreateInfo& cuboid);
        /**
         * @brief Adds a cuboid to the environment.
         * @param origin Coordinate of the lower left front-side corner.
         * @param velocity Initial velocity for all particles.
         * @param num_particles Number of particles along each dimension: N1 x N2 x N3.
         * @param width Distance between the particles.
         * @param mass Mass of each particle.
         * @param thermal_v Thermal velocity of the particles.
         * @param type Type of each particle (default: 0).
         * @param dimension Dimension of the cuboid (default: INFER).
         * @param state Initial state of the particles (default: ALIVE).
         */
        void add_cuboid(const vec3& origin, const vec3& velocity, const uint3& num_particles, double width,
            double mass, double thermal_v = 0, int type = 0, Dimension dimension = Dimension::INFER,
            Particle::State state = Particle::ALIVE);
        /**
         * @brief Adds a sphere to the environment.
         * @param sphere
         */
        void add_sphere(const SphereCreateInfo& sphere);
        /**
         * @brief Adds a sphere to the environment.
         * @param origin Coordinates of the center.
         * @param velocity Initial velocity of all particles.
         * @param radius The radius in terms of the number of molecules along the radius.
         * @param width Distance between the particles.
         * @param mass The mass of the particles.
         * @param thermal_v Thermal velocity of the particles.
         * @param type The type of each particle (default: 0).
         * @param dimension Dimension of the sphere (default: INFER).
         * @param state Initial state of the particles (default: ALIVE).
         */
        void add_sphere(const vec3& origin, const vec3& velocity, int radius, double width, double mass,
            double thermal_v = 0, int type = 0, Dimension dimension = Dimension::INFER,
            Particle::State state = Particle::ALIVE);
        /**
         * @brief Adds a membrane to the environment.
         * @param origin Coordinates of the origin.
         * @param velocity Initial velocity of the particles.
         * @param num_particles Number of particles of the membrane.
         * @param width Distance between particles.
         * @param mass Mass of the particles.
         * @param k Stiffness constant.
         * @param cutoff cutoff.
         * @param type The type of the particles (default: 0).
         */
        void add_membrane(const vec3& origin, const vec3& velocity, const uint3& num_particles, double width,
            double mass, double k, double cutoff, int type = 0);



        /**
         * @brief Computes the force between two particles.
         * @param p1 The first particle.
         * @param p2 The second particle.
         * @param pair
         * @return The force between the two particles.
         */
        [[nodiscard]] vec3 force(const Particle& p1, const Particle& p2, const CellPair & pair) const;
        /**
         * @brief Provides access to particles filtered by grid cell type and state.
         * @param type The type to filter (default: GridCell::Inside).
         * @param state The state to filter (default: (GridCell::ALIVE).
         * @return A range of particles matching the specified type and state.
         */
        auto particles(GridCell::Type type = GridCell::INSIDE, Particle::State state = Particle::ALIVE) {
            return particle_storage | std::ranges::views::filter([this, state, type](const Particle& particle) {
                return filter_particles(particle, state, type);
            });
        }
        /**
        * @brief Provides access to particles filtered by grid cell type and state (const version).
        * @param type The type to filter (default: GridCell::Inside).
        * @param state The state to filter (default: (GridCell::ALIVE).
        * @return A const range of particles matching the specified type and state.
        */
        [[nodiscard]] auto particles(GridCell::Type type = GridCell::INSIDE,
                                     Particle::State state = Particle::ALIVE) const {
            return particle_storage | std::ranges::views::filter([this, state, type](const Particle& particle) {
                return filter_particles(particle, state, type);
            });
        }
        /**
         * @brief Retrieves the linked grid cells in the simulation.
         * @return A const reference to the vector of the linked cell pairs.
         */
        const std::vector<CellPair>& linked_cells();


        /**
         * @brief Applies the boundary conditions to a particle.
         * @param particle The particle to which the conditions will be applied.
         */
        void apply_boundary(Particle& particle);

        /**
         * @brief Computes the average velocity of the particles.
         * @return The average velocity.
         */
        vec3 average_velocity();
        /**
         * @brief Calculates temperature of the system.
         * @param avg_vel The average velocity of the environment particles (default: {0, 0, 0}).
         */
        double temperature(const vec3& avg_vel = {}) const;
        /**
         * @brief Returns the boundary extent of the environment.
         * @return The boundary extent.
         */
        vec3 extent() const;
        /**
         * @brief Returns the origin of the boundary of the environment (bottom left front corner).
         * @return The origin of the boundary.
         */
        vec3 origin() const;
        /**
         * @brief Scales the thermal velocity of the particles.
         * @param scalar The scaling factor.
         * @param mean_v Mean velocity.
         */
        void scale_thermal_velocity(double scalar, vec3 mean_v={});


        /**
         * Returns the number of particles of a certain state in the environment.
         * @param state The state of the particles to count (default: Particle::ALIVE).
         * @return The number of particles.
         */
        [[nodiscard]] size_t size(Particle::State state = Particle::ALIVE) const;
        /**
         * @brief Returns the dimension of the environment.
         * @return The dimension.
         */
        [[nodiscard]] int dim() const;

        /**
         * @brief Accesses particle by its ID.
         * @param id The ID of the particle.
         * @return A reference to the particle.
         */
        Particle& operator[](size_t id);
        /**
         * @brief Accesses particle by its ID (const version).
         * @param id The ID of the particle.
         * @return A reference to the particle.
         */
        const Particle& operator[](size_t id) const;

        // making class non-copyable
        Environment(const Environment&) = delete;
        Environment& operator=(const Environment&) = delete;

    private:
        /**
         * @brief Filters particles based on their state and location within the grid.
         * @param particle The particle to be filtered.
         * @param state The state of the particle to match.
         * @param type The grid cell type to match.
         * @return "true" if the particle's state matches the given state and type, "false" otherwise.
         */
        [[nodiscard]] bool filter_particles(const Particle& particle, Particle::State state, GridCell::Type type) const;

        std::vector<Particle> particle_storage; ///< vector with all particles

        Boundary boundary;     ///< Boundary conditions of the environment.
        ParticleGrid grid;     ///< Grid of the environment.
        ForceManager forces;   ///< Forces with which the particles interact.

        Dimension dimension;  ///< Dimension of the simulation
        double grid_constant; ///< Used grid Constant in the environment.
        bool initialized;     ///< Indicates whether the environment has been initialized.
        unsigned num_stat_particles;   ///< Number of stationary particles.
    };
} // namespace md::env
