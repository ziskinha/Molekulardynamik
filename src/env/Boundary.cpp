#include "Boundary.h"

#include <stdexcept>

#include <utils/ArrayUtils.h>
#include "ParticleGrid.h"
#include "Particle.h"

namespace md::env {

    const int3 Boundary::LEFT = {-1, 0, 0};
    const int3 Boundary::RIGHT = {1, 0, 0};
    const int3 Boundary::TOP = {0, 1, 0};
    const int3 Boundary::BOTTOM = {0, -1, 0};
    const int3 Boundary::FRONT = {0, 0, 1};
    const int3 Boundary::BACK = {0, 0, -1};

    /// -----------------------------------------
    /// \brief Helper methods
    /// -----------------------------------------

    double scalar_product(const std::array<int, 3>& face_normal, const std::array<double, 3>& pos) {
        return face_normal[0] * pos[0] + face_normal[1] * pos[1] + face_normal[2] * pos[2];
    }

    std::vector<int3> decompose_normal(const int3 & normal) {
        // TODO assert components are either 1 or -1
        std::vector<int3> ret;
        ret.reserve(3);

        if (normal[0] != 0) ret.push_back({normal[0], 0, 0});
        if (normal[1] != 0) ret.push_back({0, normal[1], 0});
        if (normal[2] != 0) ret.push_back({0, 0, normal[2]});

        return ret;
    }

    int num_boundary_faces(const int3 & normal) {
        // TODO assert components are either 1 or -1
        return (normal[0] != 0) + (normal[1] != 0) + (normal[2] != 0);
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

    double distance_to_boundary(const Particle & particle, const int3 & normal, const GridCell & cell) {
        const int axis = axis_from_normal(normal);
        const double coord = particle.position[axis] - cell.origin[axis];
        if ( normal[axis] == 1) return cell.size[axis] - coord;
        if ( normal[axis] == -1) return coord;
        throw std::invalid_argument("???");
    }


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

        if (current_cell.type == GridCell::OUTSIDE) {
            int3 face_normal = previous_cell.face_normal;
            // calculate which face particle traveled through if the cell has more than one face
            if (num_boundary_faces(face_normal) > 1) {
                const vec3 diff = particle.position - particle.old_position;
                const vec3 rel_pos = particle.old_position - origin;

                const vec3 y = {
                    face_normal[0] == -1 ? 0 /*left*/ : extent[0] /*right*/,
                    face_normal[1] == -1 ? 0 /*bottom*/ : extent[1] /*top*/,
                    face_normal[2] == -1 ? 0 /*back*/ : extent[2] /*front*/,
                };

                const vec3 t = {
                    (y[0] - rel_pos[0]) / diff[0],
                    (y[1] - rel_pos[1]) / diff[1],
                    (y[2] - rel_pos[2]) / diff[2],
                };

                auto normals = decompose_normal(face_normal);

                for (int3 normal : normals) {
                    const int axis = axis_from_normal(normal);
                    const auto non_axis = non_axis_indices(axis);
                    const vec3 intersection = t[axis] * diff + rel_pos; // intersection of the particles path with the boundary

                    if (intersection[non_axis[0]] >= 0 && intersection[non_axis[0]] <= extent[non_axis[0]] &&
                        intersection[non_axis[1]] >= 0 && intersection[non_axis[1]] <= extent[non_axis[1]]) {
                        face_normal = normal;
                        break;
                    }
                }
            }

            rules[face_normal_to_idx(face_normal)](particle, face_normal, current_cell, previous_cell);
        } else {
            // apply boundary conditions for each boundary of the cell
            for (const auto &normal :  decompose_normal(current_cell.face_normal)) {
                rules[face_normal_to_idx(normal)](particle, normal, current_cell, previous_cell);
            }
        }
    }

    Boundary::BoundaryRule Boundary::Outflow() {
        return [](Particle & particle, const int3 &, const GridCell& current_cell, const GridCell& previous_cell) {
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
    Boundary::BoundaryRule Boundary::Periodic() {}
}
