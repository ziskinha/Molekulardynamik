#pragma once

#include "Common.h"

// #define BOUNDARY_OUTFLOW 1
// #define BOUNDARY_REFLECTIVE_SYMMETRIC 2
// #define BOUNDARY_REFLECTIVE_VECTOR 3
// #define BOUNDARY_REFLECTIVE_REPULSIVE 4


namespace md::env {
    struct Boundary {
        enum Type {
            OUTFLOW,
            REFLECTIVE_SYMMETRIC,
            REFLECTIVE_VECTOR,
            REFLECTIVE_REPULSIVE
        };
        using Types = std::array<Type, 6>;

        enum Face { LEFT, RIGHT, TOP, BOTTOM, FRONT, BACK };
        enum Extent { WIDTH, HEIGHT, DEPTH };

        // TODO maybe change to -1 and add if to grid::build to skip for loops. -1 would then indicate no grid
        vec3 extent {MAX_EXTENT, MAX_EXTENT, MAX_EXTENT}; // [width, height, depth]
        Types types {};    // [left, right, top, bottom, front, back]
        vec3 origin {};
    };
}