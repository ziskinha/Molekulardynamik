#pragma once

#include <limits>
#include "Common.h"

#define MAX_EXTENT std::numeric_limits<double>::max()
#define CENTER_BOUNDARY_ORIGIN std::numeric_limits<double>::max()

namespace md::env {
    class  Boundary {
    public:
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

        Boundary();


        vec3 extent {MAX_EXTENT, MAX_EXTENT, MAX_EXTENT}; // [width, height, depth]
        vec3 origin {CENTER_BOUNDARY_ORIGIN, CENTER_BOUNDARY_ORIGIN, CENTER_BOUNDARY_ORIGIN};


        Types types {OUTFLOW, OUTFLOW, OUTFLOW, OUTFLOW, OUTFLOW, OUTFLOW}; // [left, right, top, bottom, front, back]


        void apply_boundary(Particle * particle, const GridCell & cell) {
            
        }
    };
}