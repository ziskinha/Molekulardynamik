#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "env/Common.h"
#include "env/Particle.h"
#include "utils/ContainerUtils.h"

/**
 * @file ParticleGrid.cpp
 * @brief Implements the Linked cells.
 *
 * The following plot visualizes the benefit of Linked Cells:
 *
 * @image html input/images/plot.png "Performance plot" width=800px height=600px
 */

/**
 * @brief Contains classes and structures for managing the environment of the simulation.
 */
namespace md::env {
    /**
     * @brief Structure representing a cell of the particle grid.
     */
    struct GridCell {
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

        const Type type;   ///< The type of the grid cell.
        const vec3 origin; ///< The coordinates of the origin of the grid cell.
        const vec3 size;   ///< The size of the grid cell.
        const int3 idx;    ///< The index of the grid cell
        int id;            ///< The id of the grid cell.

        std::unordered_set<Particle*> particles{};   ///< The set of particles inside the grid cell.
    private:
        static int count;  ///< A counter for generating unique ids for grid cells.
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
    struct GridCellPair{
        using ParticlePairIterator = utils::DualPairIterator<std::unordered_set<Particle*>>;

        /**
         * @brief Constructs a new GridCellPair of two grid cells.
         * @param cell1 The fist grid cell.
         * @param cell2 The second grid cell.
         */
        GridCellPair(GridCell& cell1, GridCell& cell2);

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

       private:
        GridCell& cell1;  ///< The first grid cell in the pair.
        GridCell& cell2;  ///< The second grid cell in the pair.
    };

    /**
     * @brief A class representing the particle grid.
     */
    class ParticleGrid {
       public:
        ParticleGrid() = default;

        /**
         * @brief Builds the particle grid with the given parameters.
         * @param extent The size of the simulation space.
         * @param grid_const The constant used to define grid cell size.
         * @param particles The particles that fill the cells.
         * @param origin The lower left corner of the domain.
         */
        void build(const vec3& extent, double grid_const, std::vector<Particle>& particles, const vec3& origin);

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
         * @brief returns all pairs of linked cells
         * @return A vector with GridCellPairs
         */
        const std::vector<GridCellPair> & linked_cells();

        /**
         * @brief returns all cells at the boundary
         * @return A vector with GridCell pointers
         */
        const std::vector<GridCell*> & boundary_cells();

        /**
         * @brief Updates the relevant grid cells when a particle moves from one cell to another.
         * @param particle Pointer to the particle which moves.
         * @param old_cell The index of the old cell (before moving).
         * @param new_cell The index of the new cell (after moving).
         */
        void update_cells(Particle* particle, const int3& old_cell, const int3& new_cell);

        vec3 position_in_grid(const vec3& abs_position) const;
        vec3 position_in_cell(const vec3& abs_position) const;

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
        */
        void build_cell_pairs();

        std::unordered_map<int3, GridCell, Int3Hasher> cells {}; ///< A hash map storing the cells in the grid.
        std::vector<GridCellPair> cell_pairs{};                  ///< A vector of linked cell pairs.
        std::vector<GridCell*> border_cells;                     ///< A vector of cells at the domain boundary

        uint3 cell_count{};         ///< The number of cells in the grid along each dimension.
        vec3 cell_size{};           ///< The size of each grid cell.
        vec3 boundary_origin = {};  ///< The origin of the boundary.
    };
}  // namespace md::env
