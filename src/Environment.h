
#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include <unordered_set>
#include <map>
#include <ranges>
#include <limits>
#include <type_traits>

#define UINT_T uint32_t
#define INT_T int64_t
#define PARTICLE_TYPE_DEAD std::numeric_limits<int>::max()
#define MAX_EXTENT std::numeric_limits<double>::max()

namespace md {
    class Grid;
    struct Particle;

    using vec3 = std::array<double, 3>;
    using uint3 = std::array<UINT_T, 3>;
    using int3 = std::array<INT_T, 3>;
    using ForceFunc = std::function<vec3(const Particle&, const Particle&)>;

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



    struct Particle {
        enum State {
            ALIVE = 0x1,
            DEAD = 0x2,
            STATIONARY = 0x4,
        };

        Particle(size_t id, Grid& grid, const vec3& position, const vec3& velocity, double mass, int type);
        Particle(const Particle& other);

        void reset_force();
        void update_grid();

        bool operator==(const Particle& other) const;
        [[nodiscard]] std::string to_string() const;

        vec3 position;
        vec3 velocity;
        vec3 force;
        vec3 old_force;
        int3 cell;

        double mass;
        int type;  // < 0: stationary; == PARTICLE_TYPE_DEAD: dead; else: alive
        const size_t id;
    private:
        Grid & grid;
    };



    struct Boundary {
        enum class Type {
            PASSIVE,
            PERMEABLE,
            REFLECTIVE_FORCE,
            REFLECTIVE_VECTOR,
            REFLECTIVE_REPULSIVE
        };

        enum Face { LEFT, RIGHT, TOP, BOTTOM, FRONT, BACK };
        enum Extent { WIDTH, HEIGHT, DEPTH };

        vec3 extent {MAX_EXTENT, MAX_EXTENT, MAX_EXTENT}; // [width, height, depth]
        std::array<Type, 6> types {};    // [left, right, top, bottom, front, back]
    };

    struct GridCell {
        enum Type {
            INNER = 0x1,
            OUTER = 0x2,
            BOUNDARY = 0x4,
            INSIDE = INNER | BOUNDARY,
            ALL = INNER | BOUNDARY | OUTER
        };
        GridCell(const vec3& coord, const vec3& size, Type type);
        std::string to_string() const;

        const vec3 coordinate;
        const vec3 size;
        const Type type;
        std::unordered_set<Particle*> particles{};
        const int id;
    private:
        static int count;
    };


    class Grid {
    public:
        explicit Grid(const Boundary & boundary);
        void build(const vec3& extent, double grid_constant, std::vector<Particle>& particles);
        void update_cells(Particle* particle, const int3& old_cell, const int3& new_cell);

        GridCell& get_cell(const int3& idx);
        GridCell& get_cell(const Particle & particle);

        [[nodiscard]] const GridCell& get_cell(const int3& idx) const;
        [[nodiscard]] const GridCell& get_cell(const Particle & particle) const;
        [[nodiscard]] int3 what_cell(const vec3& pos) const;
        [[nodiscard]] std::vector<int3> get_cell_indices() const;

    private:
        const Boundary & boundary;
        std::map<int3, GridCell> cells{};
        uint3 cell_count {};
        double grid_constant = 0;
    };



    // Enable bitwise operations for Particle::State
    constexpr Particle::State operator|(const Particle::State lhs, const Particle::State rhs) {
        using T = std::underlying_type_t<Particle::State>;
        return static_cast<Particle::State>(static_cast<T>(lhs) | static_cast<T>(rhs));
    }

    constexpr bool operator&(const Particle::State lhs, const Particle::State rhs) {
        using T = std::underlying_type_t<Particle::State>;
        return static_cast<T>(lhs) & static_cast<T>(rhs);
    }

    // Enable bitwise operations for GridCell::Type
    constexpr GridCell::Type operator|(const GridCell::Type lhs, const GridCell::Type rhs) {
        using T = std::underlying_type_t<GridCell::Type>;
        return static_cast<GridCell::Type>(static_cast<T>(lhs) | static_cast<T>(rhs));
    }

    constexpr bool operator&(const GridCell::Type lhs, const GridCell::Type rhs) {
        using T = std::underlying_type_t<GridCell::Type>;
        return static_cast<T>(lhs) & static_cast<T>(rhs);
    }





    class Environment {
    public:
        Environment();

        void build();
        void set_grid_constant(double g);
        void set_force(const ForceFunc& force);
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
        auto particles(Particle::State state = Particle::ALIVE, GridCell::Type type = GridCell::ALL) const {
            return particle_storage | std::ranges::views::filter([this, state, type](const Particle& particle) {
                return filter_particles(particle, state, type);
            });
        }

        Particle & operator[] (size_t id);
        const Particle & operator[] (size_t id) const
        ;

        // making class non-copyable to avoid accidentally copying all the data
        Environment(const Environment&) = delete;
        Environment& operator=(const Environment&) = delete;

    private:
        bool filter_particles(const Particle& particle, Particle::State state, GridCell::Type type) const;
        std::vector<Particle> particle_storage; // TODO replace vector with a vector wrapper that emulates a vector of fixed size
        Grid grid;

        ForceFunc force_func;
        Boundary boundary;
        double grid_constant;
        bool initialized;
    };
}


