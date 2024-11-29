#pragma once

#include <string>
#include <unordered_set>
#include <vector>
#include <unordered_map>

#include "env/Common.h"
#include "env/Particle.h"
#include "utils/ContainerUtils.h"


namespace md::env {
    struct GridCell {
        enum Type {
            INNER               = 0x001,
            BOUNDARY            = 0xFF0,
            BOUNDARY_RIGHT      = 0x010,
            BOUNDARY_LEFT       = 0x020,
            BOUNDARY_TOP        = 0x040,
            BOUNDARY_BOTTOM     = 0x080,
            BOUNDARY_FRONT      = 0x100,
            BOUNDARY_BACK       = 0x200,
            OUTSIDE             = 0x002,
            INSIDE = INNER | BOUNDARY,
        };

        GridCell(const vec3& coord, const vec3& size, Type type, const int3& idx);
        [[nodiscard]] std::string to_string() const;
        bool operator==(const GridCell& other) const;

        const Type type;
        const vec3 origin;
        const vec3 size;
        const int3 idx;
        int id;

        std::unordered_set<Particle*> particles{};
    private:
        static int count;
    };

    // Enable bitwise operations for GridCell::Type
    constexpr GridCell::Type operator|(const GridCell::Type lhs, const GridCell::Type rhs) {
        using T = std::underlying_type_t<GridCell::Type>;
        return static_cast<GridCell::Type>(static_cast<T>(lhs) | static_cast<T>(rhs));
    }

    constexpr bool operator&(const GridCell::Type lhs, const GridCell::Type rhs) {
        using T = std::underlying_type_t<GridCell::Type>;
        return static_cast<T>(lhs) & static_cast<T>(rhs);
    }

    constexpr GridCell::Type& operator|=(GridCell::Type& lhs, const GridCell::Type rhs) {
        using T = std::underlying_type_t<GridCell::Type>;
        lhs = static_cast<GridCell::Type>(static_cast<T>(lhs) | static_cast<T>(rhs));
        return lhs;
    }


    struct GridCellPair{
        using ParticlePairIterator = utils::DualPairIterator<std::unordered_set<Particle*>>;

        GridCellPair(GridCell & cell1, GridCell & cell2);
        [[nodiscard]] bool empty() const;
        [[nodiscard]] ParticlePairIterator particles() const;
        [[nodiscard]] std::string to_string() const;
        [[nodiscard]] std::pair<int, int> id() const;

    private:
        GridCell & cell1;
        GridCell & cell2;
    };



    class ParticleGrid {
    public:
        ParticleGrid() = default;
        void build(const vec3 & extent, double grid_const, std::vector<Particle>& particles, const vec3 & origin);

        GridCell& get_cell(const int3& idx);
        GridCell& get_cell(const Particle & particle);

        [[nodiscard]] const GridCell& get_cell(const int3& idx) const;
        [[nodiscard]] const GridCell& get_cell(const Particle & particle) const;
        [[nodiscard]] int3 what_cell(const vec3& position) const;
        [[nodiscard]] std::vector<int3> get_cell_indices() const;

        const std::vector<GridCellPair> & linked_cells();
        const std::vector<GridCell*> & boundary_cells();

        void update_cells(Particle* particle, const int3& old_cell, const int3& new_cell);

        vec3 position_in_grid(const vec3& abs_position) const;
        vec3 position_in_cell(const vec3& abs_position) const;

    private:
        void build_cells(const vec3 & extent, double grid_constant, std::vector<Particle>& particles);
        void build_cell_pairs();

        std::unordered_map<int3, GridCell, Int3Hasher> cells {};
        std::vector<GridCellPair> cell_pairs{};
        std::vector<GridCell*> border_cells;

        uint3 cell_count {};
        vec3 cell_size {};
        vec3 boundary_origin = {};
    };
}
