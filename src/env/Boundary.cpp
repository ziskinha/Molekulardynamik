#include "Boundary.h"

#include <stdexcept>

#include "utils/ArrayUtils.h"
#include "utils/Debug.h"
#include "ParticleGrid.h"
#include "Particle.h"

namespace md::env {

    const int3 BoundaryNormal::LEFT = {-1, 0, 0};
    const int3 BoundaryNormal::RIGHT = {1, 0, 0};
    const int3 BoundaryNormal::TOP = {0, 1, 0};
    const int3 BoundaryNormal::BOTTOM = {0, -1, 0};
    const int3 BoundaryNormal::FRONT = {0, 0, 1};
    const int3 BoundaryNormal::BACK = {0, 0, -1};


    /// -----------------------------------------
    /// \brief Helper methods
    /// -----------------------------------------
    std::vector<int3> type_to_normals(const GridCell::Type type) {
        // Assert type & GridCell::Boundary
        std::vector<int3> ret;
        ret.reserve(6);

        if (type & GridCell::BOUNDARY_LEFT) ret.push_back(BoundaryNormal::LEFT);
        if (type & GridCell::BOUNDARY_RIGHT) ret.push_back(BoundaryNormal::RIGHT);
        if (type & GridCell::BOUNDARY_TOP) ret.push_back(BoundaryNormal::TOP);
        if (type & GridCell::BOUNDARY_BOTTOM) ret.push_back(BoundaryNormal::BOTTOM);
        if (type & GridCell::BOUNDARY_FRONT) ret.push_back(BoundaryNormal::FRONT);
        if (type & GridCell::BOUNDARY_BACK) ret.push_back(BoundaryNormal::BACK);

        return ret;
    }

    int axis_from_normal(const int3 & normal) {
        ASSERT(
            (normal[0] == 0 || normal[0] == 1 || normal[0] == -1) &&
            (normal[1] == 0 || normal[1] == 1 || normal[1] == -1) &&
            (normal[2] == 0 || normal[2] == 1 || normal[2] == -1) &&
            (normal[0] != 0) + (normal[1] != 0) + (normal[2] != 0) == 1,
            "The normal vector must have exactly one non-zero component that is either 1 or -1."
        );

        for (int i = 0; i < 3; i++) {
            if (normal[i] != 0) return i;
        }
        return -1;
    }

    std::array<int, 2> non_axis_indices(const int axis) {
        ASSERT(axis >= 0 && axis <= 2, "Axis must be 0, 1, or 2.");
        if (axis == 0) return {1, 2};
        if (axis == 1) return {0, 2};
        if (axis == 2) return {1, 2};
        return {-1, -1};
    }

    size_t face_normal_to_idx(const int3 & face_normal){
        // Assert whether face_normal[i] is either 1, -1, or 0 for all i = 0,1,2
        ASSERT(
            (face_normal[0] == 0 || face_normal[0] == 1 || face_normal[0] == -1) &&
            (face_normal[1] == 0 || face_normal[1] == 1 || face_normal[1] == -1) &&
            (face_normal[2] == 0 || face_normal[2] == 1 || face_normal[2] == -1),
            "Each component of face_normal must be 0, 1, or -1."
        );

        // Assert that exactly one component has magnitude 1
        ASSERT(
            (std::abs(face_normal[0]) + std::abs(face_normal[1]) + std::abs(face_normal[2]) == 1),
            "Exactly one component of face_normal must have magnitude 1."
        );

        if (face_normal[0] == -1) return 0;
        if (face_normal[0] == 1) return 1;
        if (face_normal[1] == 1) return 3;
        if (face_normal[1] == -1) return 2;
        if (face_normal[2] == 1) return 5;
        if (face_normal[2] == -1) return 4;
        throw std::invalid_argument("Invalid face normal");
    }

    double distance_to_boundary(const Particle & particle, const int3 & normal, const GridCell & cell) {
        const int axis = axis_from_normal(normal);
        const double coord = particle.position[axis] - cell.origin[axis];
        if ( normal[axis] == 1) return cell.size[axis] - coord;
        if ( normal[axis] == -1) return coord;
        throw std::invalid_argument("???");
    }



    /// -----------------------------------------
    /// \brief Boundary class methods
    /// -----------------------------------------
    Boundary::Boundary() {
        set_boundary_rule(OUTFLOW);
    };

    void Boundary::set_boundary_rule(const BoundaryRule rule) {
        for (size_t i = 0; i < rules.size(); i++) {
            rules[i] = rule;
        }
    }

    void Boundary::set_boundary_rule(const BoundaryRule rule, const int3 & face_normal) {
        rules[face_normal_to_idx(face_normal)] = rule;
    }

    void Boundary::set_boundary_force(const BoundaryForce& force) {
        this->force = force;
    }

    void Boundary::apply_boundary(Particle & particle, const GridCell& current_cell, const GridCell& previous_cell) const {

        // if particle is outside, we need to find out which boundary interface it passed through to apply the correct rule
        if (current_cell.type == GridCell::OUTSIDE) {
            const std::vector<int3> normals = type_to_normals(previous_cell.type);

            // calculate which face particle traveled through if the cell has more than one face
            if (normals.size() > 1) {
                const vec3 diff = particle.position - particle.old_position;
                const vec3 pos = particle.old_position - origin;

                const vec3 y = {
                    diff[0] < 0 ? 0 /*left*/ : extent[0] /*right*/,
                    diff[1] < 0 ? 0 /*bottom*/ : extent[1] /*top*/,
                    diff[2] < 0 ? 0 /*back*/ : extent[2] /*front*/,
                };

                const vec3 t = {
                    (y[0] - pos[0]) / diff[0],
                    (y[1] - pos[1]) / diff[1],
                    (y[2] - pos[2]) / diff[2],
                };

                const std::array possible_normals = {
                    diff[0] > 0 ? BoundaryNormal::RIGHT : BoundaryNormal::LEFT,
                    diff[1] > 0 ? BoundaryNormal::TOP : BoundaryNormal::BOTTOM,
                    diff[2] > 0 ? BoundaryNormal::FRONT : BoundaryNormal::BACK
                };

                for (const auto& normal : possible_normals) {
                    const int axis = axis_from_normal(normal);
                    const auto non_axis = non_axis_indices(axis);
                    const vec3 intersection = t[axis] * diff + pos; // intersection of the particles path with the boundary

                    // check if intersection point is valid. if so apply rule
                    if (intersection[non_axis[0]] >= 0 && intersection[non_axis[0]] <= extent[non_axis[0]] &&
                        intersection[non_axis[1]] >= 0 && intersection[non_axis[1]] <= extent[non_axis[1]]) {
                        apply_rule(normal, particle, current_cell);
                        return;
                    }
                }

            } else { // only one normal for the current cell i.e. only one boundary interface
                apply_rule(normals[0], particle, current_cell);
            }
        }else {
            // apply boundary conditions for each boundary of the cell
            for (const auto &normal :  type_to_normals(current_cell.type)) {
                apply_rule(normal, particle, current_cell);
            }
        }
    }


    BoundaryForce Boundary::LennardJonesForce(const double epsilon, const double sigma) {
        return [sigma, epsilon](const double dist) {
            if (dist < 1.1225 * sigma) {

                const double inv_dist = sigma / dist;
                const double inv_dist_2 = inv_dist * inv_dist;       // (sigma / dist)^2
                const double inv_dist_6 = inv_dist_2 * inv_dist_2 * inv_dist_2; // (sigma / dist)^6
                const double inv_dist_12 = inv_dist_6 * inv_dist_6; // (sigma / dist)^12

                return - 24 * epsilon * (inv_dist_6 - 2 * inv_dist_12) / dist;
            }
            return 0.0;
        };
    }

    BoundaryForce Boundary::InverseDistanceForce(const double cutoff, const double pre_factor, const int exponent) {
        return [=](const double dist) {
            if (dist < cutoff) return pre_factor/pow(dist, exponent);
            return 0.0;
        };
    }


    void Boundary::apply_rule(const int3& normal, Particle& particle, const GridCell& current_cell) const {
        switch (rules[face_normal_to_idx(normal)]) {
            case OUTFLOW: outflow_rule(particle, current_cell); break;
            case PERIODIC: periodic_rule(particle, normal, current_cell); break;
            case REPULSIVE_FORCE: repulsive_force_rule(particle, normal, current_cell); break;
            case VELOCITY_REFLECTION: velocity_reflection_rule(particle, normal, current_cell); break;
        }
    }


    void Boundary::outflow_rule(Particle& particle, const GridCell& previous_cell) const {
        // this rule may be called when applying all boundary conditions of a cell, hence we need a check if
        // the particle is outside or inside
        if (previous_cell.type == GridCell::OUTSIDE) {
            particle.state = Particle::DEAD;
            particle.update_grid();
        }
    }

    void Boundary::periodic_rule(Particle& particle,const int3& normal, const GridCell& current_cell) const {
        if (current_cell.type == GridCell::OUTSIDE) {
            int axis = axis_from_normal(normal);
            vec3 dx = {0,0,0};
            if (normal[axis] == 1) dx[axis] = - extent[axis];
            else if (normal[axis] == -1) dx[axis] = extent[axis];
            particle.update_position(dx);
            particle.update_grid();
        }
    }

    void Boundary::repulsive_force_rule(Particle& particle, const int3 & normal, const GridCell& cell) const {
        const double dist = distance_to_boundary(particle, normal, cell);

        if (dist != 0) { // particles may be initialized at the boundary
            vec3 df = {};
            int axis = axis_from_normal(normal);
            df[axis] = force(dist) * normal[axis];
            particle.force = particle.force - df;
        }
    }

    void Boundary::velocity_reflection_rule(Particle& particle, const int3 & normal, const GridCell& cell) const {
        if (cell.type != GridCell::OUTSIDE) return;

        const vec3 pos = particle.old_position - origin;
        const vec3 diff = particle.position - particle.old_position;
        const int axis = axis_from_normal(normal);

        const double y = normal[axis] < 0 ? 0 : extent[axis];
        const double t = (y - pos[axis]) / diff[axis];

        const vec3 intersection = t * diff + pos;
        vec3 reflected = particle.position - intersection;
        reflected[axis] = - reflected[axis];

        particle.velocity[axis] = - particle.velocity[axis];
        particle.position = reflected + intersection;
        particle.update_grid();
    }
}

