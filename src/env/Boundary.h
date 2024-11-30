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

    enum BoundaryRule {
        OUTFLOW,             ///< Particles that cross the boundary are being removed.
        PERIODIC,            ///< Particles exiting one side reenter from the opposite side.
        REPULSIVE_FORCE,     ///< Particles near the boundary are subject to a repulsive force
        VELOCITY_REFLECTION, ///< When a particle crosses the boundary, its velocity is reflected about the boundary
    };

    class  Boundary {
    public:
        enum Extent { WIDTH, HEIGHT, DEPTH };

        Boundary();
        void set_boundary_rule(BoundaryRule rule);
        void set_boundary_rule(BoundaryRule rule, const int3 & face_normal);


        void apply_boundary(Particle & particle, const GridCell& current_cell, const GridCell& previous_cell) const;

        vec3 extent {MAX_EXTENT, MAX_EXTENT, MAX_EXTENT}; // [width, height, depth]
        vec3 origin {CENTER_BOUNDARY_ORIGIN, CENTER_BOUNDARY_ORIGIN, CENTER_BOUNDARY_ORIGIN};
    private:
        void apply_rule(const int3& normal, Particle & particle, const GridCell& current_cell) const;
        std::array<BoundaryRule, 6> rules; // [left, right, top, bottom, front, back]

        void outflow_rule(Particle & particle, const GridCell& previous_cell) const;
        void periodic_rule(Particle & particle, const int3& normal, const GridCell& current_cell) const;
        void repulsive_force_rule(Particle & particle, const int3 & normal, const GridCell& cell) const;
        void velocity_reflection_rule(Particle & particle) const;
    };
}