#include "ParticleGrid.h"

#include <cmath>
#include <string>
#include <sstream>
#include <ranges>

#include "io/Logger.h"
#include "utils/ArrayUtils.h"


# define DOUBLE_MIN std::numeric_limits<double>::min()
# define DOUBLE_MAX std::numeric_limits<double>::max()
# define MIN_VEC3 vec3{DOUBLE_MIN, DOUBLE_MIN, DOUBLE_MIN}
# define MAX_VEC3 vec3{DOUBLE_MAX, DOUBLE_MAX, DOUBLE_MAX}
# define OUTSIDE_CELL int3{-1,-1,-1}

namespace md::env {
    int GridCell::count = 0;


    /// -----------------------------------------
    /// \brief Gridcell methods
    /// -----------------------------------------
    GridCell::GridCell(const vec3& coord, const vec3& size, const Type type, const int3& idx)
        : coordinate(coord), size(size), type(type), id(count++), idx(idx) {}

    std::string GridCell::to_string() const {
        std::stringstream stream;
        using ::operator<<;
        stream << "origin:" << coordinate << " size: " << size << " type: " << type << " Particles: " << particles.
            size();
        return stream.str();
    }

    bool GridCell::operator==(const GridCell& other) const {
        return id == other.id;
    }


    /// -----------------------------------------
    /// \brief Gridcellpair methods
    /// -----------------------------------------
    GridCellPair::GridCellPair(GridCell & cell1, GridCell & cell2):cell1(cell1), cell2(cell2) {

    }

    bool GridCellPair::empty() const {
        return cell1.particles.empty() || cell2.particles.empty();
    }

    GridCellPair::ParticlePairIterator GridCellPair::particles() const {
        return {cell1.particles, cell2.particles};
    }

    std::string GridCellPair::to_string() const {
        std::stringstream stream;
        using ::operator<<;
        stream << "cell1 id: " << cell1.id << " coord: " << cell1.coordinate <<
            "cell2 id: " << cell2.id << " coord: " << cell2.coordinate;
        return stream.str();
    }

    std::pair<int, int> GridCellPair::id() const {
        return {cell1.id, cell2.id};
    }


    /// -----------------------------------------
    /// \brief ParticleGrid initilization methods
    /// -----------------------------------------
    void ParticleGrid::build(const vec3 & extent, const double grid_const, std::vector<Particle>& particles, const vec3 & origin) {
        this->boundary_origin = origin;
        build_cells(extent, grid_const, particles);
        build_cell_pairs();
    }

    void ParticleGrid::build_cells(const vec3 & extent, const double grid_constant, std::vector<Particle>& particles) {
        // number of cells along each axis
        const auto num_x = static_cast<UINT_T>(ceil(extent[0] / grid_constant));
        const auto num_y = static_cast<UINT_T>(ceil(extent[1] / grid_constant));
        const auto num_z = static_cast<UINT_T>(ceil(extent[2] / grid_constant));
        cell_count = uint3{num_x, num_y, num_z};
        cell_size = {extent[0]/num_x, extent[1]/num_y, extent[2]/num_z};

        // create cells
        for (size_t x = 0; x < num_x; x++) {
            for (size_t y = 0; y < num_y; y++) {
                for (size_t z = 0; z < num_z; z++) {
                    auto type = GridCell::INNER;

                    if (x == 0 || y == 0 || z == 0 || x == num_x - 1 || y == num_y - 1 || z == num_z - 1) {
                        type = GridCell::BOUNDARY;
                    }

                    int3 idx = {static_cast<INT_T>(x),static_cast<INT_T>(y),static_cast<INT_T>(z)};
                    GridCell cell = {{cell_size[0] * static_cast<double>(x),
                                      cell_size[1] * static_cast<double>(y),
                                      cell_size[2] * static_cast<double>(z)},
                                     cell_size, type, idx};

                    cells.emplace(idx, cell);

                    SPDLOG_TRACE("Grid Cell created. index: {} Cell: {}", idx, cell.to_string());
                }
            }
        }

        // create a cell representing the "outside"
        GridCell outside = {MIN_VEC3, MAX_VEC3, GridCell::OUTER, OUTSIDE_CELL};
        cells.emplace(OUTSIDE_CELL, outside);
        SPDLOG_TRACE("Grid Cell created. index: {} Cell: {}", OUTSIDE_CELL, outside.to_string());

        // fill cells with particles
        for (auto& p : particles) {
            int3 idx = what_cell(p.position);
            cells.at(idx).particles.emplace(&p);
            p.cell = idx;
            SPDLOG_TRACE("Particle of type {} at position {} added to Cell {}", p.type, p.position, idx);
        }
    }

    void ParticleGrid::build_cell_pairs() {
        std::vector<int3> displacements;
        for (INT_T dx = -1; dx <= 1; dx++) {
            for (INT_T dy = -1; dy <= 1; dy++) {
                for (INT_T dz = -1; dz <= 1; dz++) {
                    if (dx == 0 && dy == 0 && dz == 0) continue;
                    displacements.push_back({dx, dy, dz});
                }
            }
        }

        for (UINT_T x = 0; x < cell_count[0]; x++) {
            for (UINT_T y = 0; y < cell_count[1]; y++) {
                for (UINT_T z = 0; z < cell_count[2]; z++) {
                    int3 idx1 = {static_cast<INT_T>(x),static_cast<INT_T>(y),static_cast<INT_T>(z)};
                    cell_pairs.emplace_back(cells.at(idx1), cells.at(idx1));

                    for (const auto d : displacements) {
                        int3 idx2 = idx1 + d;
                        if (idx1 >= idx2 // ensure only unique pairs are added
                            || idx2[0] < 0 || idx2[1] < 0 || idx2[2] < 0
                            || static_cast<UINT_T>(idx2[0]) >= cell_count[0]
                            || static_cast<UINT_T>(idx2[1]) >= cell_count[1]
                            || static_cast<UINT_T>(idx2[2]) >= cell_count[2]) {
                            continue;
                        }

                        cell_pairs.emplace_back(cells.at(idx1), cells.at(idx2));
                    }
                }
            }
        }
    }

    /// -----------------------------------------
    /// \brief interaction with ParticleGrid
    /// -----------------------------------------
    const GridCell& ParticleGrid::get_cell(const int3& idx) const {
        return cells.at(idx);
    }

    const GridCell& ParticleGrid::get_cell(const Particle& particle) const {
        return cells.at(what_cell(particle.position));
    }

    GridCell& ParticleGrid::get_cell(const int3& idx) {
        return cells.at(idx);
    }

    GridCell& ParticleGrid::get_cell(const Particle& particle) {
        return cells.at(what_cell(particle.position));
    }

    int3 ParticleGrid::what_cell(const vec3& position) const {
        const vec3 pos = position - boundary_origin;
        // boundary is axis aligned
        if (pos[0] < 0 || pos[1] < 0 || pos[2] < 0) {
            return OUTSIDE_CELL;
        }

        const auto x = static_cast<INT_T>(pos[0] / cell_size[0]);
        const auto y = static_cast<INT_T>(pos[1] / cell_size[1]);
        const auto z = static_cast<INT_T>(pos[2] / cell_size[2]);

        if (static_cast<UINT_T>(x) >= cell_count[0] ||
            static_cast<UINT_T>(y) >= cell_count[1] ||
            static_cast<UINT_T>(z) >= cell_count[2]) {
            return OUTSIDE_CELL;
        }

        return int3{x, y, z};
    }

    std::vector<int3> ParticleGrid::get_cell_indices() const {
        std::vector<int3> keys;
        for (const auto& key : cells | std::views::keys) {
            keys.push_back(key);
        }
        return keys;
    }

    std::vector<GridCellPair>& ParticleGrid::linked_cells() {
        return cell_pairs;
    }

    std::vector<GridCell> ParticleGrid::grid_cells() {
        std::vector<GridCell> cells;
        cells.reserve(this->cells.size());

        for (auto& [fst, snd] : this->cells) {
            cells.push_back(snd);
        }

        return cells;
    }

    void ParticleGrid::update_cells(Particle* particle, const int3& old_cell, const int3& new_cell) {
        if (old_cell != new_cell) {
            auto& old = cells.at(old_cell);
            auto& current = cells.at(new_cell);

            old.particles.erase(particle);
            current.particles.insert(particle);

            SPDLOG_TRACE("Particle at {} changed cells from {} to {}", particle->position, old_cell, new_cell);
        }
    }

}