#pragma once

#include <limits>

#include "Common.h"

#define MAX_EXTENT std::numeric_limits<double>::max()
#define CENTER_BOUNDARY_ORIGIN std::numeric_limits<double>::max()

/**
 * @brief Contains classes and structures for managing the environment of the simulation.
 */
namespace md::env {
    /**
     * @brief Structure to represent the boundary of the simulation.
     */
    struct Boundary {
        /**
         * @brief Enumeration of possible boundary conditions.
         */
        enum Type {
            OUTFLOW,               ///< Particles that cross the boundary are being removed.
            REFLECTIVE_SYMMETRIC,  ///< TODO: complete description
            REFLECTIVE_VECTOR,     ///< TODO: complete description
            REFLECTIVE_REPULSIVE,  ///< TODO: complete description
            PERIODIC               ///< Particles exiting one side reenter from the opposite side.
        };

        using Types = std::array<Type, 6>;

        /**
         * @brief Enumeration of boundary faces.
         */
        enum Face { LEFT, RIGHT, TOP, BOTTOM, FRONT, BACK };

        /**
         * @brief Enumeration of boundary extents.
         */
        enum Extent { WIDTH, HEIGHT, DEPTH };

        vec3 extent{MAX_EXTENT, MAX_EXTENT, MAX_EXTENT};  ///< Boundary extents: [width, height, depth].
        Types types{OUTFLOW, OUTFLOW, OUTFLOW,
                    OUTFLOW, OUTFLOW, OUTFLOW};  ///< Boundary conditions for each face of the boundary:
                                                 ///< [left, right, top, bottom, front, back]
        vec3 origin{CENTER_BOUNDARY_ORIGIN, CENTER_BOUNDARY_ORIGIN,
                    CENTER_BOUNDARY_ORIGIN};  ///< Origin of the boundary.
    };
}  // namespace md::env