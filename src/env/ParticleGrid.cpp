#include "ParticleGrid.h"

#include <cmath>
#include <ranges>
#include <sstream>
#include <string>

#include "io/Logger/Logger.h"
#include "io/input/xml/molSimSchema.hxx"
#include "utils/ArrayUtils.h"

#define DOUBLE_MIN std::numeric_limits<double>::min()
#define DOUBLE_MAX std::numeric_limits<double>::max()
#define MIN_VEC3 \
    vec3 { DOUBLE_MIN, DOUBLE_MIN, DOUBLE_MIN }
#define MAX_VEC3 \
    vec3 { DOUBLE_MAX, DOUBLE_MAX, DOUBLE_MAX }
#define OUTSIDE_CELL \
    int3 { -1, -1, -1 }

namespace md::env {
    int GridCell::count = 0;

    /// -----------------------------------------
    /// \brief Grid cell methods
    /// -----------------------------------------
    GridCell::GridCell(const vec3& coord, const vec3& size, Type type, const int3& idx)
        : type(type), origin(coord), size(size), idx(idx), id(count++) {
        particles.max_load_factor(0.8);
#ifdef _OPENMP
        SPDLOG_DEBUG("Lock initialized for cell {}", id);
        omp_init_lock(&lock);
#endif
    }

    std::string GridCell::to_string() const {
        std::stringstream stream;
        using ::operator<<;
        stream << "origin:" << origin << " size: " << size << " type: " << type << " Particles: " << particles.size();
        return stream.str();
    }

    bool GridCell::operator==(const GridCell& other) const { return id == other.id; }

    void GridCell::lock_cell() {
#ifdef _OPENMP
        SPDLOG_DEBUG("Lock set for cell {}", id);
        omp_set_lock(&lock);
#endif
    }

    void GridCell::unlock_cell() {
#ifdef _OPENMP
        SPDLOG_DEBUG("Lock unset for cell {}", id);
        omp_unset_lock(&lock);
#endif
    }

    /// -----------------------------------------
    /// \brief Grid cell pair methods
    /// -----------------------------------------
    CellPair::CellPair(GridCell& cell1, GridCell& cell2, const Periodicity periodicity) :
    periodicity(periodicity), cell1(cell1), cell2(cell2) {}

    bool CellPair::empty() const { return cell1.particles.empty() || cell2.particles.empty(); }

    CellPair::ParticlePairIterator CellPair::particles() const {
        return {cell1.particles, cell2.particles};
    }

    std::string CellPair::to_string() const {
        std::stringstream stream;
        using ::operator<<;
        stream << "cell1 id: " << cell1.id << " coord: " << cell1.origin <<
            "cell2 id: " << cell2.id << " coord: " << cell2.origin;
        return stream.str();
    }

    std::pair<int, int> CellPair::id() const { return {cell1.id, cell2.id}; }

    ParticleGrid::ParticleGrid() {
        cells.max_load_factor(0.4);
    }

    /// -----------------------------------------
    /// \brief ParticleGrid initilization methods
    /// -----------------------------------------
void ParticleGrid::build(const Boundary & boundary, const double grid_const, std::vector<Particle>& particles,
                         bool build_blocks) {
        this->boundary_origin = boundary.origin;
        build_cells(boundary.extent, grid_const, particles);

        if (build_blocks) { // Build blocks as well, which are necessary for spatial decomposition parallelization
            build_cell_pairs_and_blocks(boundary.boundary_rules());
        } else {
            build_cell_pairs(boundary.boundary_rules());
        }
    }

    void ParticleGrid::build_cells(const vec3& extent, const double grid_constant, std::vector<Particle>& particles) {
        // number of cells along each axis
        const auto num_x = static_cast<UINT_T>(std::max(1.0, floor(extent[0] / grid_constant)));
        const auto num_y = static_cast<UINT_T>(std::max(1.0, floor(extent[1] / grid_constant)));
        const auto num_z = static_cast<UINT_T>(std::max(1.0, floor(extent[2] / grid_constant)));
        cell_count = uint3{num_x, num_y, num_z};
        cell_size = {extent[0] / num_x, extent[1] / num_y, extent[2] / num_z};
        size_t num_cells = num_x * num_y * num_z;

        // create cells
        for (size_t x = 0; x < num_x; x++) {
            for (size_t y = 0; y < num_y; y++) {
                for (size_t z = 0; z < num_z; z++) {
                    auto type = GridCell::INNER;

                    if (x==0) type |= GridCell::BOUNDARY_LEFT;
                    if (y==0) type |= GridCell::BOUNDARY_BOTTOM;
                    if (z==0) type |= GridCell::BOUNDARY_BACK;
                    if (x==num_x-1) type |= GridCell::BOUNDARY_RIGHT;
                    if (y==num_y-1) type |= GridCell::BOUNDARY_TOP;
                    if (z==num_z-1) type |= GridCell::BOUNDARY_FRONT;

                    int3 idx = {static_cast<INT_T>(x),static_cast<INT_T>(y),static_cast<INT_T>(z)};

                    GridCell cell = {{cell_size[0] * static_cast<double>(x),
                                      cell_size[1] * static_cast<double>(y),
                                      cell_size[2] * static_cast<double>(z)},
                                     cell_size,
                                     type,
                                     idx};
                    cell.particles.reserve(4*particles.size()/num_cells);
                    cells.emplace(idx, cell);


                    if (type == GridCell::BOUNDARY) border_cells.push_back(&cells.at(idx));

                    SPDLOG_TRACE("Grid Cell created. index: {} Cell: {}", idx, cell.to_string());
                }
            }
        }

        // create a cell representing the "outside"
        GridCell outside = {MIN_VEC3, MAX_VEC3, GridCell::OUTSIDE, OUTSIDE_CELL};
        cells.emplace(OUTSIDE_CELL, outside);
        SPDLOG_TRACE("Grid Cell created. index: {} Cell: {}", OUTSIDE_CELL, outside.to_string());

        // fill cells with particles
        for (auto& p : particles) {
            if (p.state == Particle::DEAD) continue;
            int3 idx = what_cell(p.position);
            cells.at(idx).particles.emplace(&p);
            p.cell = idx;
            SPDLOG_TRACE("Particle of type {} at position {} added to Cell {}", p.type, p.position, idx);
        }
    }

    bool wrap_around(int &x, const unsigned N) {
        const int n = static_cast<int>(N);
        if (x >= n) {
            x = x - n;
            return true;
        }
        if (x < 0) {
            x = x + n;
            return true;
        }
        return false;
    }


    std::vector<int3> ParticleGrid::compute_displacements() {
        std::vector<int3> displacements;
        for (INT_T dx = -1; dx <= 1; dx++) {
            for (INT_T dy = -1; dy <= 1; dy++) {
                for (INT_T dz = -1; dz <= 1; dz++) {
                    if (dx == 0 && dy == 0 && dz == 0) continue;
                    displacements.push_back({dx, dy, dz});
                }
            }
        }

        return displacements;
    }


    int3 ParticleGrid::compute_block_distribution () {
        int3 distribution = {1, 1, 1};
        UINT_T n_blocks = 1;
        UINT_T n_threads = 4;
#ifdef _OPENMP
        n_threads = omp_get_max_threads();
#endif

        while (n_blocks < n_threads) {
            // Find direction to increment
            int max_dim = -1;
            for (int i = 0; i < 3; ++i) {
                if (static_cast<UINT_T>(distribution[i]) < cell_count[i]
                    && (max_dim == -1 || cell_count[i] / distribution[i] > cell_count[max_dim] / distribution[max_dim])) {

                    distribution[i]++;
                    n_blocks = distribution[0] * distribution[1] * distribution[2];

                    if (n_blocks <= n_threads) {
                        max_dim = i;
                    }
                    distribution[i]--;
                }
            }
            if (max_dim == -1) break;
            distribution[max_dim]++;
        }

        return distribution;
    }


    void ParticleGrid::build_blocks(std::vector<bool> periodic_directions) {
        blocks.resize(4);
        int3 block_distribution = compute_block_distribution();

        // build normal blocks
        int x_compensation = cell_count[0] % block_distribution[0];
        int y_compensation = cell_count[1] % block_distribution[1];
        int z_compensation = cell_count[2] % block_distribution[2];

        int3 current_xyz = {0, 0, 0};

        for (int x = 0; x < block_distribution[0]; ++x) {
            int x_extend = (cell_count[0] / block_distribution[0]) + (x_compensation-- > 0 ? 1 : 0);
            for (int y = 0; y < block_distribution[1]; ++y) {
                int y_extend = (cell_count[1] / block_distribution[1]) + (y_compensation-- > 0 ? 1 : 0);
                for (int z = 0; z < block_distribution[2]; ++z) {
                    int z_extend = (cell_count[2] / block_distribution[2]) + (z_compensation-- > 0 ? 1 : 0);

                    Block block;
                    block.origin = {current_xyz[0], current_xyz[1], current_xyz[2]};
                    block.extent = {x_extend, y_extend, z_extend};
                    block.idx = {x, y, z};
                    blocks[0].push_back(block);

                    current_xyz[2] += z_extend;
                }
                current_xyz[2] = 0;
                current_xyz[1] += y_extend;
                z_compensation = cell_count[2] % block_distribution[2];
            }
            current_xyz[1] = 0;
            current_xyz[0] += x_extend;
            y_compensation = cell_count[1] % block_distribution[1];
            z_compensation = cell_count[2] % block_distribution[2];
        }

        // build communication blocks (x-axis, y-axis, z-axis)
        for (int i = 0; i < 3; ++i) {
            // If some blocks contain only one linked cell along this axis, only one communication block is needed.
            // because they wouldn't have enough distance to avoid race conditions.
            int block_num = cell_count[i] / block_distribution[i] >= 2 ?
                    periodic_directions[i] ? block_distribution[i] : block_distribution[i] - 1 : 1;

            for (int n = 0; n < block_num; ++n) {
                blocks[i + 1].emplace_back();
            }
        }
    }


    void ParticleGrid::build_cell_pairs(const std::array<BoundaryRule, 6> & rules) {
        std::vector<int3> displacements = compute_displacements();

        const bool periodic_x = rules[Boundary::LEFT] == PERIODIC;
        const bool periodic_y = rules[Boundary::TOP] == PERIODIC;
        const bool periodic_z = rules[Boundary::FRONT] == PERIODIC;

        // add cell pairs
        for (UINT_T x = 0; x < cell_count[0]; x++) {
            for (UINT_T y = 0; y < cell_count[1]; y++) {
                for (UINT_T z = 0; z < cell_count[2]; z++) {
                    int3 idx1 = {static_cast<INT_T>(x), static_cast<INT_T>(y), static_cast<INT_T>(z)};
                    cell_pairs.emplace_back(cells.at(idx1), cells.at(idx1),  CellPair::PERIODIC_NONE);

                    for (const auto d : displacements) {
                        CellPair::Periodicity periodicity = CellPair::PERIODIC_NONE;
                        int3 idx2 = idx1 + d;

                        // ensure pairs are added when periodic conditions are applied
                        if (periodic_x && wrap_around(idx2[0], cell_count[0])) periodicity |=  CellPair::PERIODIC_X;
                        if (periodic_y && wrap_around(idx2[1], cell_count[1])) periodicity |=  CellPair::PERIODIC_Y;
                        if (periodic_z && wrap_around(idx2[2], cell_count[2])) periodicity |=  CellPair::PERIODIC_Z;

                        if (idx1 >= idx2  // ensure only unique pairs are added
                            || idx2[0] < 0 || idx2[1] < 0 || idx2[2] < 0 ||
                            static_cast<UINT_T>(idx2[0]) >= cell_count[0] ||
                            static_cast<UINT_T>(idx2[1]) >= cell_count[1] ||
                            static_cast<UINT_T>(idx2[2]) >= cell_count[2]) {
                            continue;
                        }

                        cell_pairs.emplace_back(cells.at(idx1), cells.at(idx2), periodicity);
                    }
                }
            }
        }
    }

    int ParticleGrid::get_communication_block_index(Block block, int axis, int3 idx2) {
        if (static_cast<UINT_T>(idx2[axis]) == cell_count[axis] - 1 || idx2[axis] == 0 || blocks[axis + 1].size() == 1) {
            return 0;
        }
        else if (idx2[axis] < block.origin[axis]) {
            return blocks[axis + 1].size() > block.idx[axis] ? block.idx[axis] : 0;
        }
        else {
            return blocks[axis + 1].size() > block.idx[axis] + 1 ? block.idx[axis] + 1 : 0;
        }
    }

    void ParticleGrid::build_cell_pairs_and_blocks(const std::array<BoundaryRule, 6> & rules) {
        std::vector<bool> periodic_directions = {
                rules[Boundary::LEFT] == PERIODIC,
                rules[Boundary::TOP] == PERIODIC,
                rules[Boundary::FRONT] == PERIODIC
        };

        build_blocks(periodic_directions);

        // Build cell pairs and assign to corresponding blocks
        std::vector<int3> displacements = compute_displacements();

        for (auto &block : blocks[0]) {
            for (int x = block.origin[0]; x < block.origin[0] + block.extent[0]; ++x) {
                for (int y = block.origin[1]; y < block.origin[1] + block.extent[1]; ++y) {
                    for (int z = block.origin[2]; z < block.origin[2] + block.extent[2]; ++z) {
                        int3 idx1 = {static_cast<INT_T>(x), static_cast<INT_T>(y), static_cast<INT_T>(z)};
                        block.cell_pairs.emplace_back(cells.at(idx1), cells.at(idx1),  CellPair::PERIODIC_NONE);

                        for (const auto d : displacements) {
                            CellPair::Periodicity periodicity = CellPair::PERIODIC_NONE;
                            int3 idx2 = idx1 + d;

                            // ensure pairs are added when periodic conditions are applied
                            if (periodic_directions[0] && wrap_around(idx2[0], cell_count[0])) periodicity |=  CellPair::PERIODIC_X;
                            if (periodic_directions[1] && wrap_around(idx2[1], cell_count[1])) periodicity |=  CellPair::PERIODIC_Y;
                            if (periodic_directions[2] && wrap_around(idx2[2], cell_count[2])) periodicity |=  CellPair::PERIODIC_Z;

                            // TODO anpassen
                            if (idx1 >= idx2  // ensure only unique pairs are added
                                || idx2[0] < 0 || idx2[1] < 0 || idx2[2] < 0 ||
                                static_cast<UINT_T>(idx2[0]) >= cell_count[0] ||
                                static_cast<UINT_T>(idx2[1]) >= cell_count[1] ||
                                static_cast<UINT_T>(idx2[2]) >= cell_count[2]) {
                                continue;
                            }

                            // assign CellPair to corresponding block and set
                            if (idx2[0] < block.origin[0] || idx2[0] >= block.origin[0] + block.extent[0]) {
                                // communication_blocks_x
                                int x_block_idx = get_communication_block_index(block, 0, idx2);
                                blocks[1][x_block_idx].cell_pairs.emplace_back(cells.at(idx1), cells.at(idx2), periodicity);
                            }
                            else if (idx2[1] < block.origin[1] || idx2[1] >= block.origin[1] + block.extent[1]) {
                                // communication_blocks_y
                                int y_block_idx = get_communication_block_index(block, 1, idx2);
                                blocks[2][y_block_idx].cell_pairs.emplace_back(cells.at(idx1), cells.at(idx2), periodicity);
                            }
                            else if (idx2[2] < block.origin[2] || idx2[2] >= block.origin[2] + block.extent[2]) {
                                // communication_blocks_z
                                int z_block_idx = get_communication_block_index(block, 2, idx2);
                                blocks[3][z_block_idx].cell_pairs.emplace_back(cells.at(idx1), cells.at(idx2), periodicity);
                            }
                            else {
                                // normal blocks
                                block.cell_pairs.emplace_back(cells.at(idx1), cells.at(idx2), periodicity);
                            }
                        }
                    }
                }
            }
        }
    }

    /// -----------------------------------------
    /// \brief interaction with ParticleGrid
    /// -----------------------------------------
    const GridCell& ParticleGrid::get_cell(const int3& idx) const { return cells.at(idx); }

    const GridCell& ParticleGrid::get_cell(const Particle& particle) const {
        return cells.at(what_cell(particle.position));
    }

    GridCell& ParticleGrid::get_cell(const int3& idx) { return cells.at(idx); }

    GridCell& ParticleGrid::get_cell(const Particle& particle) { return cells.at(what_cell(particle.position)); }

    int3 ParticleGrid::what_cell(const vec3& position) const {
        const vec3 pos = position_in_grid(position);
        // boundary is axis aligned
        if (pos[0] < 0 || pos[1] < 0 || pos[2] < 0) {
            return OUTSIDE_CELL;
        }

        const auto x = static_cast<INT_T>(pos[0] / cell_size[0]);
        const auto y = static_cast<INT_T>(pos[1] / cell_size[1]);
        const auto z = static_cast<INT_T>(pos[2] / cell_size[2]);

        if (static_cast<UINT_T>(x) >= cell_count[0] || static_cast<UINT_T>(y) >= cell_count[1] ||
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

    const std::vector<CellPair>& ParticleGrid::linked_cells() {
        return cell_pairs;
    }

    const std::vector<std::vector<Block>>& ParticleGrid::block_sets() {
        return blocks;
    }

    const std::vector<GridCell*> & ParticleGrid::boundary_cells() {
        return border_cells;
    }

    size_t ParticleGrid::particle_count() const {
        size_t count = 0;
        for (const auto& cell : cells | std::views::values) {
            count += cell.particles.size();
        }
        return count;
    }

    void ParticleGrid::update_cells(Particle* particle, const int3& old_cell, const int3& new_cell) {
        if (old_cell != new_cell) {
            auto& old = cells.at(old_cell);
            auto& current = cells.at(new_cell);

            old.particles.erase(particle);
            current.particles.insert(particle);

            SPDLOG_TRACE("Particle at {} changed cells from {} to {}", particle->position, old_cell, new_cell);
        }

        if (particle->state == Particle::DEAD) {
            auto& cell = cells.at(old_cell);
            cell.particles.erase(particle);
        }
    }

    vec3 ParticleGrid::position_in_grid(const vec3& abs_position) const {
        return abs_position - boundary_origin;
    }

    vec3 ParticleGrid::position_in_cell(const vec3& abs_position) const {
        const vec3 grid_pos = position_in_grid(abs_position);
        const GridCell & cell = cells.at(what_cell(abs_position));
        return grid_pos - cell.origin;
    }
} //namespace md::env
