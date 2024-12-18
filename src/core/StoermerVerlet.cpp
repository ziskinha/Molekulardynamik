#include "StoermerVerlet.h"

#include <iostream>
#include <utility>

#include "io/Logger/Logger.h"
#include "utils/ArrayUtils.h"

namespace md::Integrator {

    void StoermerVerlet::simulation_step(unsigned step, const double dt) {

        // update position
        for (auto& p : env.particles()) {
            p.update_position(dt * p.velocity + pow(dt, 2) / (2 * p.mass) * p.force);
            p.update_grid();
            p.reset_force();
        }


        for (auto & particle : env.particles(env::GridCell::BOUNDARY | env::GridCell::OUTSIDE)) {
            env.apply_boundary(particle);
        }
        
        // calculate forces
        for (auto& cell_pair : env.linked_cells()) {
            auto it = cell_pair.particles();
            for (auto [p1, p2] : cell_pair.particles()) {
                // std::cout << p1->id << " " << p2->id << std::endl;
                // if (p1 == 0x7fb483d25e10 && p2 == 0x39003900390039) {
                //
                // }
                if (p1->id == 672 && p2->id == 686) {
                    int i = 0;
                }
                // vec3 new_F = env.force(*p1, *p2, cell_pair);
                //
                // p2->force = p2->force + new_F;
                // p1->force = p1->force - new_F;
            }
        }

        // apply gravity
        for (auto& p : env.particles()) {
            p.force = p.force + env.gravity_force(p);
        }

        // update velocities
        for (auto& p : env.particles()) {
            p.velocity = p.velocity + dt / 2 / p.mass * (p.force + p.old_force);
        }

        // apply thermostat
        if (step % temp_adjust_freq == 0) {
            thermostat.adjust_temperature(env);
        }

    }

}  // namespace md::Integrator
