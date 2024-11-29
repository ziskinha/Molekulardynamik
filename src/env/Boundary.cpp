#include "Boundary.h"

#include <stdexcept>

#include <utils/ArrayUtils.h>
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
        // TODO assert only one component is either 1 or -1. The rest is 0
        for (int i = 0; i < 3; i++) {
            if (normal[i] != 0) return i;
        }
        return -1;
    }

    std::array<int, 2> non_axis_indices(const int axis) {
        // TODO assert axis = 0,1,2
        if (axis == 0) return {1, 2};
        if (axis == 1) return {0, 2};
        if (axis == 2) return {1, 2};
        return {-1, -1};
    }

    size_t face_normal_to_idx(const int3 & face_normal){
        // TODO Assert whether face_normal[i] = 1 or = -1 for all i in [0,2]
        // TODO Assert that only one component has magnitude 1

        if (face_normal[0] == -1) return 0;
        if (face_normal[0] == 1) return 1;
        if (face_normal[1] == 1) return 3;
        if (face_normal[1] == -1) return 2;
        if (face_normal[2] == 1) return 5;
        if (face_normal[2] == -1) return 4;
        throw std::invalid_argument("Invalid face normal");
    }




    /// -----------------------------------------
    /// \brief Boundary class methods
    /// -----------------------------------------
    Boundary::Boundary() {
        set_boundary_rule(Outflow());
    };

    void Boundary::set_boundary_rule(BoundaryRule rule) {
        for (size_t i = 0; i < rules.size(); i++) {
            rules[i] = rule;
        }
    }

    void Boundary::set_boundary_rule(BoundaryRule rule, const int3 & face_normal) {
        rules[face_normal_to_idx(face_normal)] = rule;
    }

    void Boundary::apply_boundary(Particle & particle, const GridCell& current_cell, const GridCell& previous_cell) const {
        // TODO handle case when the system consists of only a single cell (this cell has 6 boundary faces)

        const std::vector<int3> normals = type_to_normals(current_cell.type);

        // if particle is outside, we need to find out which boundary interface it passed through to apply the correct rule
        if (current_cell.type == GridCell::OUTSIDE) {
            // calculate which face particle traveled through if the cell has more than one face
            if (normals.size() > 1) {
                const vec3 diff = particle.position - particle.old_position;
                const vec3 rel_pos = particle.old_position - origin;

                const vec3 y = {
                    diff[0] > 0 ? 0 /*left*/ : extent[0] /*right*/,
                    diff[1] > 0 ? 0 /*bottom*/ : extent[1] /*top*/,
                    diff[2] > 0 ? 0 /*back*/ : extent[2] /*front*/,
                };

                const vec3 t = {
                    (y[0] - rel_pos[0]) / diff[0],
                    (y[1] - rel_pos[1]) / diff[1],
                    (y[2] - rel_pos[2]) / diff[2],
                };

                const std::array possible_normals = {
                    diff[0] > 0 ? BoundaryNormal::RIGHT : BoundaryNormal::LEFT,
                    diff[1] > 0 ? BoundaryNormal::TOP : BoundaryNormal::BOTTOM,
                    diff[2] > 0 ? BoundaryNormal::FRONT : BoundaryNormal::BACK
                };

                for (const auto& normal : possible_normals) {
                    const int axis = axis_from_normal(normal);
                    const auto non_axis = non_axis_indices(axis);
                    const vec3 intersection = t[axis] * diff + rel_pos; // intersection of the particles path with the boundary

                    // check if intersection point is valid. if so apply rule
                    if (intersection[non_axis[0]] >= 0 && intersection[non_axis[0]] <= extent[non_axis[0]] &&
                        intersection[non_axis[1]] >= 0 && intersection[non_axis[1]] <= extent[non_axis[1]]) {
                        apply_rule(normal, particle, current_cell, previous_cell);
                        return;
                    }
                }

            } else { // only one normal for the current cell i.e. only one boundary interface
                apply_rule(normals[0], particle, current_cell, previous_cell);
            }
        }else {
            // apply boundary conditions for each boundary of the cell
            for (const auto &normal :  normals) {
                apply_rule(normal, particle, current_cell, previous_cell);
            }
        }
    }


    void Boundary::apply_rule(const int3& normal, Particle& particle, const GridCell& current_cell,
           const GridCell& previous_cell) const {
        rules[face_normal_to_idx(normal)](particle, normal, current_cell, previous_cell);
    }




    /// -----------------------------------------
    /// \brief Boundary Rules
    /// -----------------------------------------

    double distance_to_boundary(const Particle & particle, const int3 & normal, const GridCell & cell) {
        const int axis = axis_from_normal(normal);
        const double coord = particle.position[axis] - cell.origin[axis];
        if ( normal[axis] == 1) return cell.size[axis] - coord;
        if ( normal[axis] == -1) return coord;
        throw std::invalid_argument("???");
    }

    Boundary::BoundaryRule Boundary::Outflow() {
        return [](Particle & particle, const int3 &, const GridCell& current_cell, const GridCell&) {
            if (current_cell.type == GridCell::OUTSIDE) {
                particle.state = Particle::DEAD;
                particle.update_grid();
            }
        };
    }

    Boundary::BoundaryRule Boundary::VirtualParticleRepulsion() {}
    Boundary::BoundaryRule Boundary::VectorReflection() {}

    Boundary::BoundaryRule Boundary::UniformRepulsiveForce() {
        return [](Particle & particle, const int3 & normal ,const GridCell& current_cell, const GridCell&) {
            const double dist = distance_to_boundary(particle, normal, current_cell);

            if (dist != 0) {
                const double force_mag = 1/dist/dist;
                particle.force = particle.force - vec3{force_mag * normal[0], force_mag * normal[1], force_mag * normal[2]};
            }
        };
    }
    Boundary::BoundaryRule Boundary::Periodic() {

    }

}

