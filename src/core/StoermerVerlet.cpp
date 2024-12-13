#include "StoermerVerlet.h"

#include <iostream>
#include <utility>

#include "io/Logger/Logger.h"
#include "utils/ArrayUtils.h"

namespace md::Integrator {

    void StoermerVerlet::simulation_step(unsigned step, const double dt, double& modifications ) {
    void StoermerVerlet::simulation_step(unsigned step, const double dt, double& modifications ) {
        // update position
        for (auto& p : env.particles()) {
            p.update_position(dt * p.velocity + pow(dt, 2) / (2 * p.mass) * p.old_force);
            modifications++;
            modifications++;
            p.update_grid();
            p.reset_force();
        }

        for (auto & particle : env.particles(env::GridCell::BOUNDARY | env::GridCell::OUTSIDE)) {
            env.apply_boundary(particle);
        }

        // calculate forces
        for (auto& cell_pair : env.linked_cells()) {
            for (auto [p1, p2] : cell_pair.particles()) {
                vec3 new_F = env.force(*p1, *p2, cell_pair);

                p2->force = p2->force + new_F;
                p1->force = p1->force - new_F;
                modifications+=2;
                modifications+=2;
            }
        }

        // apply gravity
        for (auto& p : env.particles()) {
            p.force = p.force + env.gravity_force(p);
            modifications++;
            modifications++;
        }

        // update velocities
        for (auto& p : env.particles()) {
            p.velocity = p.velocity + dt / 2 / p.mass * (p.force + p.old_force);
            modifications++;
            modifications++;
        }

        // apply thermostat
        if (step % temp_adjust_freq == 0) {
            thermostat.adjust_temperature(env,modifications);
            thermostat.adjust_temperature(env,modifications);
        }
    }
}  // namespace md::Integrator
