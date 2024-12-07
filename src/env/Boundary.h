#pragma once

#include <limits>

#include "Common.h"
#include "Force.h"


#define MAX_EXTENT std::numeric_limits<double>::max()
#define CENTER_BOUNDARY_ORIGIN std::numeric_limits<double>::max()

/**
 * @brief Contains classes and structures for managing the environment of the simulation.
 */
namespace md::env {
    struct Particle;
    struct GridCell;

    /**
     * @brief A structure representing the normal vectors of the boundary.
     */
    struct BoundaryNormal {
        static const int3 LEFT;    ///< Normal vector of the left face.
        static const int3 RIGHT;   ///< Normal vector of the right face.
        static const int3 TOP;     ///< Normal vector of the top face.
        static const int3 BOTTOM;  ///< Normal vector of the bottom face.
        static const int3 FRONT;   ///< Normal vector of the front face.
        static const int3 BACK;    ///< Normal vector of the back face.
    };


    /**
    * @brief Enumeration of possible boundary conditions.
    */
    enum BoundaryRule {
        OUTFLOW,             ///< Particles that cross the boundary are being removed.
        PERIODIC,            ///< Particles exiting one side reenter from the opposite side.
        REPULSIVE_FORCE,     ///< Particles near the boundary are subject to a repulsive force.
        VELOCITY_REFLECTION, ///< When a particle crosses the boundary, its velocity is reflected about the boundary.
    };

    using BoundaryForce = std::function<double(double)>;

    /**
     * @brief Class for managing the boundaries of the simulation.
     */
    class  Boundary {
    public:
        /**
         * @brief Enumeration of the spatial extents of the simulation space.
         */
        enum Extent {
            WIDTH,   ///< Width of the simulation space.
            HEIGHT,  ///< Height of the simulation space.
            DEPTH    ///< Depth of the simulation space.
        };

        enum Face {
            LEFT,
            RIGHT,
            TOP,
            BOTTOM,
            FRONT,
            BACK
       };

        static Face normal_to_face(const int3 & normal);

        /**
         * @brief Constructs a default Boundary object.
         */
        Boundary();

        /**
         * @brief Sets the boundary rule for all faces.
         * @param rule The rule to be applied.
         */
        void set_boundary_rule(BoundaryRule rule);

        /**
         * @brief Sets the boundary rule for a specific face.
         * @param rule The rule to be applied.
         * @param face_normal The normal vector of the face where the rule is applied.
         */
        void set_boundary_rule(BoundaryRule rule, const int3 &face_normal);

        /**
         * @brief Sets a force to be applied at the boundary.
         * @param force A callable object defining the force.
         */
        void set_boundary_force(const BoundaryForce &force);

        /**
         * @brief Calculates the face and applies the boundary conditions to a particle.
         * @param particle The particle to which the conditions are applied.
         * @param current_cell The particle's current gird cell.
         * @param previous_cell The particle's previous gird cell.
         */
        void apply_boundary(Particle &particle, const GridCell &current_cell, const GridCell &previous_cell) const;

        [[nodiscard]] const std::array<BoundaryRule, 6> & boundary_rules() const;

        // TODO set to private
        vec3 extent{MAX_EXTENT, MAX_EXTENT, MAX_EXTENT}; ///< Dimensions of the boundary [width, height, depth].
        vec3 origin{CENTER_BOUNDARY_ORIGIN, CENTER_BOUNDARY_ORIGIN, CENTER_BOUNDARY_ORIGIN}; ///< origin of the boundary.

        /**
         * @brief Creates a Lennard-Jones boundary force.
         * @param epsilon
         * @param sigma
         * @return A callable object representing the Lennard-Jones force.
         */
        static BoundaryForce LennardJonesForce(double epsilon, double sigma);

        /**
         * @brief Creates an inverse distance boundary force.
         * @param cutoff
         * @param pre_factor
         * @param exponent
         * @return A callable object representing the inverse distance force.
         */
        static BoundaryForce InverseDistanceForce(double cutoff, double pre_factor, int exponent = 2);

        // functions for querying whether the force has been set if required
        [[nodiscard]] bool requires_force_function() const;
        [[nodiscard]] bool has_force_function() const;

    private:
        /**
         * @brief Applies a boundary rule to a particle based on the boundary face it interacts with.
         * @param normal The normal vector of the boundary face.
         * @param particle The particle to which the rule is applied.
         * @param current_cell The particle's current grid cell.
         */
        void apply_rule(const int3 &normal, Particle &particle, const GridCell &current_cell) const;

        /**
         * @brief Implements the outflow rule.
         * (Particles that cross the boundary are removed).
         * @param particle The particle in question.
         * @param previous_cell The particle's previous cell.
         */
        void outflow_rule(Particle &particle, const GridCell &previous_cell) const;

        /**
         * @brief Implements the periodic rule.
         * (Particles exiting one side reenter from the opposite side).
         * @param particle The particle in question.
         * @param normal The normal vector of the boundary face.
         * @param current_cell The particle's current cell.
         */
        void periodic_rule(Particle &particle, const int3 &normal, const GridCell &current_cell) const;

        /**
         * @brief Implements the repulsive force rule.
         * (Particles near the boundary are subject to a repulsive force).
         * @param particle The particle in question.
         * @param normal The normal vector of the boundary face.
         * @param cell The particle's cell.
         */
        void repulsive_force_rule(Particle &particle, const int3 &normal, const GridCell &cell) const;

        /**
         * @brief Implements the velocity reflection rule.
         * (When a particle crosses the boundary, its velocity is reflected about the boundary).
         * @param particle The particle in question.
         * @param normal The normal vector of the boundary face.
         * @param cell The particle's call.
         */
        void velocity_reflection_rule(Particle &particle, const int3 &normal, const GridCell &cell) const;

        std::array<BoundaryRule, 6> rules{}; ///< Rules applied to each face of the boundary [left, right, top, bottom, front, back]
        BoundaryForce force;  ///< The applied force at the boundary.
    };
} //namespace md::env