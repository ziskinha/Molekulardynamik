
#pragma once

#include <array>
#include <cstdint>
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
     * @brief Structure containing information required to create a particle.
     */
    struct ParticleCreateInfo {
        /**
         * @brief Constructs a ParticleCreateInfo instance.
         * @param position Position vector of the particle.
         * @param velocity Velocity vector of the particle.
         * @param mass Mass of particle (default: 0).
         * @param type Type of particle (default: 0).
         */
        ParticleCreateInfo(const vec3& position, const vec3& velocity, double mass, int type = 0);
        vec3 position{};
        vec3 velocity{};
        double mass = 0;
        int type = 0;
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
         */
        CuboidCreateInfo(const vec3& origin, const vec3& initial_v, const uint3& num_particles, double thermal_v,
                         double width, double mass, uint8_t dimension, int type = 0);
        vec3 origin;
        vec3 initial_v;
        uint3 num_particles;
        double thermal_v{};
        double width{};
        double mass{};
        uint8_t dimension{};
        int type = 0;
    };

    /**
     * @brief Structure containing information required to create a sphere of particles.
     */
    struct SphereCreateInfo {
        /**
         * @brief Constructs a SphereCreateInfo.
         * @param origin Coordinates of the center.
         * @param initial_v Initial velocity of all particles.
         * @param thermal_v
         * @param radius The radius in terms of the number of molecules along the radius.
         * @param width Distance between the particles.
         * @param mass The mass of the particles.
         * @param dimension
         * @param type The type of each particles (dafault: 0).
         */
        SphereCreateInfo(const vec3& origin, const vec3& initial_v, const double thermal_v, int radius, double width,
                         double mass, const uint8_t dimension, int type = 0);
        vec3 origin;
        vec3 initial_v;
        double thermal_v;
        int radius;
        double width;
        double mass;
        uint8_t dimension{};
        int type;
    };

    /**
     * @brief Number of dimensions for the simulation
     */
    enum Dimension {
        TWO = 2,
        THREE = 3,
        INFER = -1
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
         * @brief Sets the grid constant, the desired side length of a cell
         * @param g New grid constant.
         */
        void set_grid_constant(double g);

        /**
         * @brief Sets the force with which particles interact
         * @param force The force function to be used.
         */
        void set_force(const Force& force);

        /**
         * @brief Sets the boundary conditions for the environment.
         * @param boundary The boundary condition to be used.
         */
        void set_boundary(const Boundary& boundary);

        /**
        * @brief Sets the dimension of the environment
        * @param dim
        */
        void set_dimension(Dimension dim);

        void set_gravity_constant(double g);

        /**
         * @brief Adds a single particle to the environment.
         * @param position Position of the particle.
         * @param velocity Velocity of the particle.
         * @param mass Mass of the particle.
         * @param type Type of the particle.
         */
        void add_particle(const vec3& position, const vec3& velocity, double mass, int type = 0);

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
         * @param initial_v Initial velocity for all particles.
         * @param num_particles Number of particles along each dimension: N1 x N2 x N3.
         * @param thermal_v Thermal velocity of the particles.
         * @param width Distance between the particles.
         * @param mass Mass of each particle.
         * @param dimension Dimension of the cuboid.
         * @param type Type of each particle (default: 0).
         */
        void add_cuboid(const vec3& origin, const vec3& initial_v, const uint3& num_particles, double thermal_v,
                        double width, double mass, uint8_t dimension, int type = 0);

        /**
         * @brief Adds a sphere to the environment.
         * @param sphere
         */
        void add_sphere(const SphereCreateInfo& sphere);

        /**
         * @brief Adds a sphere to the environment.
         * @param origin Coordinates of the center.
         * @param initial_v Initial velocity of all particles.
         * @param thermal_v Thermal velocity of the particles.
         * @param radius The radius in terms of the number of molecules along the radius.
         * @param width Distance between the particles.
         * @param mass The mass of the particles.
         * @param dimension Dimension of the sphere.
         * @param type The type of each particle (default: 0).
         */
        void add_sphere(const vec3& origin, const vec3& initial_v, double thermal_v, int radius, double width,
                        double mass, uint8_t dimension, int type = 0);


        /**
         * @brief Computes the force between two particles.
         * @param p1 The first particle.
         * @param p2 The second particle.
         * @return The force between the two particles.
         */
        [[nodiscard]] vec3 force(const Particle& p1, const Particle& p2, const CellPair & pair) const;

        /**
         * Returns the number of particles of a certain state in the environment.
         * @param state The state of the particles to count (default: Particle::ALIVE).
         * @return The number of particles.
         */
        [[nodiscard]] size_t size(Particle::State state = Particle::ALIVE) const;

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

        vec3 gravity_force(const Particle& particle) const;

        /**
         * @brief Calculate temperature of the system
         */
        double temperature() const;

        int dim() const;

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

        // TODO replace vector with a vector wrapper that emulates a vector of fixed size
        std::vector<Particle> particle_storage; ///< vector with all particles

        Boundary boundary;     ///< Boundary conditions of the environment.
        ParticleGrid grid;     ///< Grid of the environment.

        Force force_func;     ///< Used force function in the environment.
        Dimension dimension;  ///< Dimension of the simulation
        double grid_constant; ///< Used grid Constant in the environment.
        bool initialized;     ///< Indicates whether the environment has been initialized.
        double g_grav;
    };
} // namespace md::env
