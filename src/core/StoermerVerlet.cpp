#include "StoermerVerlet.h"

#include <iostream>
#include <utility>

#include "io/Logger.h"
#include "utils/ArrayUtils.h"

namespace md::Integrator {

    void dummy_run_simulation(StoermerVerlet& obj, double dt) { obj.simulation_step(dt); }

    void StoermerVerlet::simulation_step(const double dt) {
        // update position
        for (auto& p : env.particles()) {
            p.update_position(dt * p.velocity + pow(dt, 2) / (2 * p.mass) * p.old_force);
            p.update_grid();
            p.reset_force();
        }

        for (auto & particle : env.particles(env::GridCell::BOUNDARY | env::GridCell::OUTSIDE)) {
            env.apply_boundary(particle);
        }

        // calculate forces
        for (auto& cell_pair : env.linked_cells()) {
            for (auto [p1, p2] : cell_pair.particles()) {
                vec3 new_F = env.force(*p1, *p2);

                p2->force = p2->force + new_F;
                p1->force = p1->force - new_F;
            }
        }

        // update velocities
        for (auto& p : env.particles()) {
            p.velocity = p.velocity + dt / 2 / p.mass * (p.force + p.old_force);
        }
    }
}  // namespace md::Integrator
