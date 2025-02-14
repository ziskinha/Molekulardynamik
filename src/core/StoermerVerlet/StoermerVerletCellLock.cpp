#include "StoermerVerletCellLock.h"

#include "utils/ArrayUtils.h"

namespace md::Integrator {

    void StoermerVerletCellLock::simulation_step(const unsigned step, const double dt) {
        // update position
        for (auto &p: env.particles()) {
            p.update_position(dt * p.velocity + pow(dt, 2) / (2 * p.mass) * p.force);
            p.update_grid();
            p.reset_force();
        }

        for (auto &particle: env.particles(env::GridCell::BOUNDARY | env::GridCell::OUTSIDE)) {
            env.apply_boundary(particle);
        }

        // calculate forces
#pragma omp parallel for
        for (size_t i = 0; i < env.linked_cells().size(); ++i) {
            auto &cell_pair = env.linked_cells()[i];
            cell_pair.cell1.lock_cell();

            if (cell_pair.cell1.id == cell_pair.cell2.id) {
                auto &particles = cell_pair.cell1.particles;
                for (auto it1 = particles.begin(); it1 != particles.end(); ++it1) {
                    for (auto it2 = std::next(it1); it2 != particles.end(); ++it2) {
                        env::Particle *p1 = *it1;
                        env::Particle *p2 = *it2;

                        vec3 new_F = env.force(*p1, *p2, cell_pair);
                        p2->force = p2->force + new_F;
                        p1->force = p1->force - new_F;
                    }
                }
            } else {
                cell_pair.cell2.lock_cell();
                for (auto *p1: cell_pair.cell1.particles) {
                    for (auto *p2: cell_pair.cell2.particles) {
                        vec3 new_F = env.force(*p1, *p2, cell_pair);
                        p2->force = p2->force + new_F;
                        p1->force = p1->force - new_F;
                    }
                }
                cell_pair.cell2.unlock_cell();
            }
            cell_pair.cell1.unlock_cell();
        }

        // apply constant forces
        for (auto &f: external_forces) {
            const std::vector<size_t> marked_particles = f.marked_particles();
#pragma omp parallel for
            for (size_t i = 0; i < marked_particles.size(); ++i) {
                f.apply_force(env[marked_particles[i]], dt * step);
            }
        }

        // update velocities
#pragma omp parallel for
        for (size_t i = 0; i < (env.size(env::Particle::ALIVE | env::Particle::STATIONARY | env::Particle::DEAD)); ++i) {
            auto &p = env[i];
            if (p.state == env::Particle::ALIVE) {
                p.velocity = p.velocity + dt / 2 / p.mass * (p.force + p.old_force);
            }
        }

        // apply thermostat
        if (step % temp_adjust_freq == 0) {
            thermostat.adjust_temperature(env);
        }

    }

} // namespace md::Integrator
