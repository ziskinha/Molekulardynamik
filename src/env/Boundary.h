#pragma once

#include <limits>
#include "Common.h"

#define MAX_EXTENT std::numeric_limits<double>::max()
#define CENTER_BOUNDARY_ORIGIN std::numeric_limits<double>::max()

namespace md::env {

    // enum BoundaryType {
    //     OUTFLOW,
    //     REFLECTIVE_SYMMETRIC,
    //     REFLECTIVE_VECTOR,
    //     REFLECTIVE_REPULSIVE,
    //     PERIODIC
    // };

    class  Boundary {
    public:
        // using Types = std::array<Type, 6>;

        using BoundaryRule = std::function<void(Particle*, const GridCell&)>;

        enum Face { LEFT, RIGHT, TOP, BOTTOM, FRONT, BACK };
        enum Extent { WIDTH, HEIGHT, DEPTH };

        Boundary();


        vec3 extent {MAX_EXTENT, MAX_EXTENT, MAX_EXTENT}; // [width, height, depth]
        vec3 origin {CENTER_BOUNDARY_ORIGIN, CENTER_BOUNDARY_ORIGIN, CENTER_BOUNDARY_ORIGIN};


        // Types types {OUTFLOW, OUTFLOW, OUTFLOW, OUTFLOW, OUTFLOW, OUTFLOW}; // [left, right, top, bottom, front, back]
        std::array<BoundaryRule, 6> types; // [left, right, top, bottom, front, back]

        void apply_boundary(Particle * particle, const GridCell & cell) {

        }
    };
}