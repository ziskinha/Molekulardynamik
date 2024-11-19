//
// Created by jdyma on 26/10/2024.
//

#include "StoermerVerlet.h"

#include <utility>

#include "io/Logger.h"
#include "utils/ArrayUtils.h"

namespace md::Integrator {


    void dummy_run_simulation(StoermerVerlet& obj, double dt) { obj.simulation_step(dt); }

    void StoermerVerlet::simulation_step(const double dt) {

        // update position
        for (auto& p : system.particles(Particle::ALIVE)) {
            p.update_position(dt * p.velocity + pow(dt, 2) / (2 * p.mass) * p.old_force);
        }

        for (auto& p : system.particles(Particle::ALIVE)) {
            p.reset_force();
        }
        // calculate forces
        for (auto& p1 : system.particles(Particle::ALIVE)) {
            for (auto& p2 : system.particles(Particle::ALIVE)) {
                vec3 new_F = system.force(p1, p2);

                p2.force = p2.force + new_F;
            }
        }

        // for (auto& cell_pair : system.linked_cells) {
        //     for (auto& [p1, p2] : system.particles(cell_pair)) {
        //         vec3 new_F = system.force(p1, p2);
        //
        //         p2.force = p2.force + new_F;
        //         p1.force = p1.force - new_F;
        //     }
        // }

        // boundary conditions
        // for (auto & particle : system.particles(Particle::ALIVE, GridCell::BOUNDARY)) {
        //
        // }

        // update velocities
        for (auto& p : system.particles(Particle::ALIVE)) {
            p.update_velocity(dt / 2 / p.mass * (p.force + p.old_force));
        }
    }
}  // namespace md::Integrator
