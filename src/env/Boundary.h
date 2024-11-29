#pragma once

#include <limits>
#include "Common.h"
#include "Force.h"
#include "ParticleGrid.h"


#define MAX_EXTENT std::numeric_limits<double>::max()
#define CENTER_BOUNDARY_ORIGIN std::numeric_limits<double>::max()

namespace md::env {

    struct Particle;
    struct GridCell;

    struct BoundaryNormal {
        static const int3 LEFT;
        static const int3 RIGHT;
        static const int3 TOP;
        static const int3 BOTTOM;
        static const int3 FRONT;
        static const int3 BACK;
    };


    class  Boundary {
    public:
        using BoundaryRule = std::function<void(Particle&, const int3 &, const GridCell&, const GridCell&)>;

        enum Extent { WIDTH, HEIGHT, DEPTH };

        Boundary();
        void set_boundary_rule(BoundaryRule rule);
        void set_boundary_rule(BoundaryRule rule, const int3 & face_normal);

        vec3 extent {MAX_EXTENT, MAX_EXTENT, MAX_EXTENT}; // [width, height, depth]
        vec3 origin {CENTER_BOUNDARY_ORIGIN, CENTER_BOUNDARY_ORIGIN, CENTER_BOUNDARY_ORIGIN};

        void apply_boundary(Particle & particle, const GridCell& current_cell, const GridCell& previous_cell) const;

        static BoundaryRule Outflow();
        static BoundaryRule VirtualParticleRepulsion();
        static BoundaryRule VectorReflection();
        static BoundaryRule UniformRepulsiveForce();
        static BoundaryRule Periodic();

    private:
        void apply_rule(const int3& normal, Particle & particle, const GridCell& current_cell, const GridCell& previous_cell) const;
        std::array<BoundaryRule, 6> rules; // [left, right, top, bottom, front, back]
    };
}