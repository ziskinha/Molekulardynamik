#pragma once

#include <limits>
#include "Common.h"

#define MAX_EXTENT std::numeric_limits<double>::max()

namespace md::env {
    struct Boundary {
        enum Type {
            OUTFLOW,
            REFLECTIVE_SYMMETRIC,
            REFLECTIVE_VECTOR,
            REFLECTIVE_REPULSIVE,
            PERIODIC
        };
        using Types = std::array<Type, 6>;

        enum Face { LEFT, RIGHT, TOP, BOTTOM, FRONT, BACK };
        enum Extent { WIDTH, HEIGHT, DEPTH };

        vec3 extent {MAX_EXTENT, MAX_EXTENT, MAX_EXTENT}; // [width, height, depth]
        Types types {OUTFLOW, OUTFLOW, OUTFLOW, OUTFLOW, OUTFLOW, OUTFLOW}; // [left, right, top, bottom, front, back]
        vec3 origin {-MAX_EXTENT/2, -MAX_EXTENT/2, -MAX_EXTENT/2};
    };
}