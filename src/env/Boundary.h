#pragma once

#include "Common.h"

namespace md::env {
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

        // TODO maybe change to -1 and add if to grid::build to skip for loops. -1 would then indicate no grid
        vec3 extent {MAX_EXTENT, MAX_EXTENT, MAX_EXTENT}; // [width, height, depth]
        std::array<Type, 6> types {};    // [left, right, top, bottom, front, back]
    };
}