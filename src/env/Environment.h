
#pragma once

#include <array>
#include <cstdint>
#include <vector>
#include <ranges>

#include "Common.h"
#include "Particle.h"
#include "ParticleGrid.h"
#include "Force.h"
#include "Boundary.h"


#define GRID_CONSTANT_AUTO 0


namespace md::env {
    struct ParticleCreateInfo {
        ParticleCreateInfo(const vec3& position, const vec3& velocity, double mass, int type = 0);
        vec3 position{};
        vec3 velocity{};
        double mass = 0;
        int type = 0;
    };


    struct CuboidCreateInfo {
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


    class Environment {
    public:
        Environment();

        void build();
        void set_grid_constant(double g);
        void set_force(const Force& force);
        void set_boundary(const Boundary& boundary);
        void add_particle(const vec3& position, const vec3& velocity, double mass, int type = 0);
        void add_particles(const std::vector<ParticleCreateInfo>& particles);
        void add_cuboid(const CuboidCreateInfo& cuboid);
        void add_cuboid(const vec3& origin, const vec3& initial_v, const uint3& num_particles, double thermal_v,
                         double width, double mass, uint8_t dimension, int type = 0);

        [[nodiscard]] vec3 force(const Particle& p1, const Particle& p2) const;
        [[nodiscard]] size_t size(Particle::State state = Particle::ALIVE) const;

        auto particles(Particle::State state = Particle::ALIVE, GridCell::Type type = GridCell::ALL) {
            return particle_storage | std::ranges::views::filter([this, state, type](const Particle& particle) {
                return filter_particles(particle, state, type);
            });
        }
        [[nodiscard]] auto particles(Particle::State state = Particle::ALIVE, GridCell::Type type = GridCell::ALL) const {
            return particle_storage | std::ranges::views::filter([this, state, type](const Particle& particle) {
                return filter_particles(particle, state, type);
            });
        }

        std::vector<GridCellPair> & linked_cells();
        std::vector<GridCell> cells();


        Particle & operator[] (size_t id);
        const Particle & operator[] (size_t id) const;

        // making class non-copyable
        Environment(const Environment&) = delete;
        Environment& operator=(const Environment&) = delete;

    private:
        [[nodiscard]] bool filter_particles(const Particle& particle, Particle::State state, GridCell::Type type) const;

        std::vector<Particle> particle_storage; // TODO replace vector with a vector wrapper that emulates a vector of fixed size
        Boundary boundary;
        ParticleGrid grid;

        Force force_func;
        double grid_constant;
        bool initialized;
    };
}


