#pragma once

#include <string>
#include <vector>
#include <omp.h>

#include "ankerl/unordered_dense.h"
#include "env/Common.h"
#include "env/Particle.h"
#include "env/Boundary.h"
#include "utils/ContainerUtils.h"

/**
 * @brief Contains classes and structures for managing the environment of the simulation.
 */
namespace md::env {
    struct PointerHash {
        size_t operator()(const void* ptr) const noexcept {
            return reinterpret_cast<uintptr_t>(ptr) >> 3; // Right shift to improve distribution
        }
    };
    /**
     * @brief Structure representing a cell of the particle grid.
     */
    struct GridCell {
        using particle_container = ankerl::unordered_dense::set<Particle*, PointerHash>;

        /**
         * @brief Enumeration of the type of the grid cell.
         */
        enum Type {
            INNER               = 0x001,  ///< A cell inside the grid.
            BOUNDARY            = 0xFF0,  ///< A boundary cell.
            BOUNDARY_RIGHT      = 0x010,  ///< A boundary cell on the right side.
            BOUNDARY_LEFT       = 0x020,  ///< A boundary cell on the left side.
            BOUNDARY_TOP        = 0x040,  ///< A boundary cell on the top side.
            BOUNDARY_BOTTOM     = 0x080,  ///< A boundary cell on the bottom side.
            BOUNDARY_FRONT      = 0x100,  ///< A boundary cell on the front side.
            BOUNDARY_BACK       = 0x200,  ///< A boundary cell on the back side.
            OUTSIDE             = 0x002,  ///< A cell outside the grid.
            INSIDE = INNER | BOUNDARY,    ///< Cells considered inside the boundary
        };

        /**
         * @brief Constructs a new GridCell with the given parameters.
         * @param coord The coordinates of the cell.
         * @param size The size of the cell.
         * @param type The type of the grid cell.
         * @param idx The index of the grid cell.
         */
        GridCell(const vec3& coord, const vec3& size, Type type, const int3& idx);

        /**
         * @brief Creates a string representation of the grid cell's properties.
         * @return A string describing the grid cell.
         */
        [[nodiscard]] std::string to_string() const;

        /**
         * @brief Compares the cell to another regarding equality.
         * @param other The other GirdCell object to compare with.
         * @return "true" if the grid cells are equal, "false" otherwise.
         */
        bool operator==(const GridCell& other) const;

        /**
         * @brief locks the lock of cell.
         */
        void lock_cell();
        /**
         * @brief unlocks the lock of the cell
         */
        void unlock_cell();

        const Type type;   ///< The type of the grid cell.
        const vec3 origin; ///< The coordinates of the origin of the grid cell.
        const vec3 size;   ///< The size of the grid cell.
        const int3 idx;    ///< The index of the grid cell
        int id;            ///< The id of the grid cell.

        particle_container particles{}; ///< The set of particles inside the grid cell.
    private:
        static int count;  ///< A counter for generating unique ids for grid cells.
#ifdef _OPENMP
        omp_lock_t lock;
#endif
    };

    /**
     * @brief Enables bitwise OR operation for GridCell::Type.
     * @param lhs
     * @param rhs
     * @return The result of the bitwise OR operation between lhs and rhs.
     */
    constexpr GridCell::Type operator|(const GridCell::Type lhs, const GridCell::Type rhs) {
        using T = std::underlying_type_t<GridCell::Type>;
        return static_cast<GridCell::Type>(static_cast<T>(lhs) | static_cast<T>(rhs));
    }

    /**
     * @brief Enables bitwise AND operation for GridCell::Type.
     * @param lhs
     * @param rhs
     * @return "true" if bitwise AND operation between lhs and rhs is non-zero, "false" otherwise.
     */
    constexpr bool operator&(const GridCell::Type lhs, const GridCell::Type rhs) {
        using T = std::underlying_type_t<GridCell::Type>;
        return static_cast<T>(lhs) & static_cast<T>(rhs);
    }

    /**
     * @brief Enables bitwise OR assignment for GridCell::Type.
     * @param lhs
     * @param rhs
     * @return The updated lhs.
     */
    constexpr GridCell::Type& operator|=(GridCell::Type& lhs, const GridCell::Type rhs) {
        using T = std::underlying_type_t<GridCell::Type>;
        lhs = static_cast<GridCell::Type>(static_cast<T>(lhs) | static_cast<T>(rhs));
        return lhs;
    }




    /**
     * @brief Structure representing a pair of linked grid cells
     */
    struct CellPair{
        /**
         * @brief Enumeration representing the periodicity, used for applying periodic boundary condition.
         */
        enum Periodicity {
            PERIODIC_NONE    = 0x0,  ///< No periodicity.
            PERIODIC_X       = 0x1,  ///< Periodicity in the x-direction.
            PERIODIC_Y       = 0x2,  ///< Periodicity in the y-direction.
            PERIODIC_Z       = 0x4,  ///< Periodicity in the z-direction.
        };

        using ParticlePairIterator = utils::DualPairIterator<GridCell::particle_container>;

        /**
         * @brief Constructs a new GridCellPair of two grid cells.
         * @param cell1 The fist grid cell.
         * @param cell2 The second grid cell.
         * @param periodicity The periodicity.
         */
        CellPair(GridCell& cell1, GridCell& cell2, Periodicity periodicity);

        /**
         * @brief Checks if the GridCellPair is empty.
         * @return "true" if both cells are empty, "false" otherwise.
         */
        [[nodiscard]] bool empty() const;

        /**
         * @brief Constructs an iterator for iterating over pairs of particles.
         * @return An iterator for the pairs of particles.
         */
        [[nodiscard]] ParticlePairIterator particles() const;

        /**
         * @brief Creates a string representation of GridCellPair.
         * @return A string describing the GridCellPair.
         */
        [[nodiscard]] std::string to_string() const;

        /**
         * @brief Retrieves the ids of the cells.
         * @return A pair of integers representing the ids.
         */
        [[nodiscard]] std::pair<int, int> id() const;

        const Periodicity periodicity;
        GridCell& cell1;  ///< The first grid cell in the pair.
        GridCell& cell2;  ///< The second grid cell in the pair.
    };

    /**
     * @brief Enables bitwise OR operation for combining periodicity flags.
     * @param lhs
     * @param rhs
     * @return The combined periodicity condition.
     */
    inline CellPair::Periodicity operator|(const CellPair::Periodicity lhs, const CellPair::Periodicity rhs) {
        using T = std::underlying_type_t<CellPair::Periodicity>;
        return static_cast<CellPair::Periodicity>(static_cast<T>(lhs) | static_cast<T>(rhs));
    }

    /**
     * @brief Enables bitwise OR assignment for periodicity flags.
     * @param lhs
     * @param rhs
     * @return A reference to the modified periodicity condition.
     */
    inline CellPair::Periodicity& operator|=(CellPair::Periodicity &lhs, CellPair::Periodicity rhs) {
        using T = std::underlying_type_t<CellPair::Periodicity>;
        lhs = static_cast<CellPair::Periodicity>(static_cast<T>(lhs) | static_cast<T>(rhs));
        return lhs;
    }

    /**
     * @brief Enables bitwise AND operator for periodicity flags.
     * @param lhs
     * @param rhs
     * @return The intersection of the periodicity conditions.
     */
    inline CellPair::Periodicity operator&(const CellPair::Periodicity lhs, const CellPair::Periodicity rhs) {
        using T = std::underlying_type_t<CellPair::Periodicity>;
        return static_cast<CellPair::Periodicity>(static_cast<T>(lhs) & static_cast<T>(rhs));
    }


    /**
     * @brief Structure representing a block used for spatial decomposition parallelization.
     */
    struct Block {
        int3 origin;   ///< The origin of the block.
        int3 extent;   ///< The extent of the block.
        int3 idx;      ///< The index of the block.
        std::vector<CellPair> cell_pairs;   ///< The cell pairs lying within the block.
    };


    /**
     * @brief A class representing the particle grid.
     */
    class ParticleGrid {
       public:
        ParticleGrid();

        /**
         * @brief Builds the particle grid with the given parameters.
         * @param boundary
         * @param grid_const The constant used to define grid cell size.
         * @param particles The particles that fill the cells.
         */
        void build(const Boundary & boundary, double grid_const, std::vector<Particle>& particles, bool build_blocks = false);

        /**
         * @brief Retrieves the grid cell corresponding the index.
         * @param idx The index of the grid cell.
         * @return A reference to the grid cell at the index.
         */
        GridCell& get_cell(const int3& idx);

        /**
         * @brief Retrieves the grid cell corresponding to the particle.
         * @param particle The particle.
         * @return A reference to the grid cell containing the particle.
         */
        GridCell& get_cell(const Particle& particle);

        /**
         * @brief Retrieves the grid cell corresponding the index (const version).
         * @param idx The index of the grid cell.
         * @return A reference to the grid cell at the index.
         */
        [[nodiscard]] const GridCell& get_cell(const int3& idx) const;

        /**
         * @brief Retrieves the grid cell corresponding to the particle (const version).
         * @param particle The particle.
         * @return A reference to the grid cell containing the particle.
         */
        [[nodiscard]] const GridCell& get_cell(const Particle& particle) const;

        /**
         * @brief Determines the index of the grid cell of the position.
         * @param position The position to check.
         * @return The index of the grid cell containing the position.
         */
        [[nodiscard]] int3 what_cell(const vec3& position) const;

        /**
         * @brief Retrieves all the grid cells indices.
         * @return A vector containing the grid cell indices.
         */
        [[nodiscard]] std::vector<int3> get_cell_indices() const;

        /**
         * @brief Computes the displacements of cells in the environment.
         * @return A vector of displacements.
         */
        std::vector<int3> compute_displacements();

        /**
         * @brief Computes the distribution of blocks for parallelization based on the number of threads. Blocks are
         * built along the linked cells.
         * @return The block distribution as an `int3` (representing the number of blocks along each axis).
         */
        int3 compute_block_distribution ();

        /**
         * @brief Builds the blocks for parallelization, considering periodic boundary conditions.
         * @param periodic_directions A vector indicating which directions are periodic.
         */
        void build_blocks(std::vector<bool> periodic_directions);

        /**
         * @brief Calculates the index of the communication block to which the pair should be assigned.
         * @param block The normal block in which one cell of the pair is currently located.
         * @param axis The axis along which communication is being considered.
         * @param idx2 The index of the second element in the pair.
         * @return The index of the communication block.
         */
        int get_communication_block_index(Block block, int axis, int3 idx2);

        /**
         * @brief returns all pairs of linked cells
         * @return A vector with GridCellPairs
         */
        const std::vector<CellPair> & linked_cells();

        /**
         * @brief returns the block sets.
         *
         * The returned block sets include:
         * [0]: Normal blocks,
         * [1]: Communication blocks along the x-axis,
         * [2]: Communication blocks along the y-axis,
         * [3]: Communication blocks along the z-axis.
         *
         * @return A reference to the vector of vectors of blocks.
         */
        const std::vector<std::vector<Block>> & block_sets();

        /**
         * @brief returns all cells at the boundary
         * @return A vector with GridCell pointers
         */
        const std::vector<GridCell*> & boundary_cells();

        size_t particle_count() const;

        /**
         * @brief Updates the relevant grid cells when a particle moves from one cell to another.
         * @param particle Pointer to the particle which moves.
         * @param old_cell The index of the old cell (before moving).
         * @param new_cell The index of the new cell (after moving).
         */
        void update_cells(Particle* particle, const int3& old_cell, const int3& new_cell);

        vec3 position_in_grid(const vec3& abs_position) const;

    private:
        /**
        * @brief Builds the grid cells based on the given extent and grid constant.
        * @param extent The size of the simulation space.
        * @param grid_constant The constant used to define grid cell size.
        * @param particles The particles that fill the cells.
        */
        void build_cells(const vec3 & extent, double grid_constant, std::vector<Particle>& particles);

        /**
        * @brief Builds pairs of neighboring cells.
        * @param rules The boundary rules of the environment.
        */
        void build_cell_pairs(const std::array<BoundaryRule, 6> & rules);

        /**
         * @brief Builds pairs of neighboring cells and assigns them to the corresponding blocks.
         * @param rules The boundary rules of the environment.
         */
        void build_cell_pairs_and_blocks(const std::array<BoundaryRule, 6> & rules);

        // TODO dead code
        // std::unordered_map<int3, GridCell, Int3Hasher> cells {}; ///< A hash map storing the cells in the grid.
        ankerl::unordered_dense::map<int3, GridCell, Int3Hasher> cells{};
        std::vector<CellPair> cell_pairs{};                  ///< A vector of linked cell pairs.
        std::vector<GridCell*> border_cells;                 ///< A vector of cells at the domain boundary
        // [0]: normal blocks, [1]: communication_blocks_x, [2]: communication_blocks_y, [3]: communication_blocks_z
        std::vector<std::vector<Block>> blocks;

        uint3 cell_count{};         ///< The number of cells in the grid along each dimension.
        vec3 cell_size{};           ///< The size of each grid cell.
        vec3 boundary_origin = {};  ///< The origin of the boundary.
    };
}  // namespace md::env
