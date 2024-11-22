#pragma once

#include <string>
#include <unordered_set>
#include <vector>
#include <unordered_map>

#include "env/Common.h"
#include "env/Particle.h"


namespace md::env {

    struct Particle;

    struct GridCell {
        enum Type {
            INNER = 0x1,
            OUTER = 0x2,
            BOUNDARY = 0x4,
            INSIDE = INNER | BOUNDARY,
            ALL = INNER | BOUNDARY | OUTER
        };
        // GridCell()
        // : coordinate{0.0, 0.0, 0.0}, size{0.0, 0.0, 0.0}, type(ALL), id(count++) {}

        GridCell(const vec3& coord, const vec3& size, Type type);
        [[nodiscard]] std::string to_string() const;

        const vec3 coordinate;
        const vec3 size;
        const Type type;
        std::unordered_set<Particle*> particles{};
        int id;
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



    struct GridCellPair {
        GridCellPair(GridCell & cell1, GridCell & cell2);

    private:
        GridCell & cell1;
        GridCell & cell2;
    };

    class ParticleGrid {
    public:
        ParticleGrid() = default;
        void build(const vec3 & extent, double grid_const, std::vector<Particle>& particles, double force_cutoff);
        void update_cells(Particle* particle, const int3& old_cell, const int3& new_cell);

        GridCell& get_cell(const int3& idx);
        GridCell& get_cell(const Particle & particle);

        [[nodiscard]] const GridCell& get_cell(const int3& idx) const;
        [[nodiscard]] const GridCell& get_cell(const Particle & particle) const;
        [[nodiscard]] int3 what_cell(const vec3& pos) const;
        [[nodiscard]] std::vector<int3> get_cell_indices() const;

    private:
        void build_cells(const vec3 & extent, double grid_const, std::vector<Particle>& particles);
        void build_cell_pairs(double force_cutoff);
        std::unordered_map<int3, GridCell, Int3Hasher> cells {};
        std::vector<GridCellPair> cell_pairs{};
        uint3 cell_count {};
        double grid_constant = 0;
    };
}