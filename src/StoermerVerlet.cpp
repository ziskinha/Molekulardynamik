//
// Created by jdyma on 26/10/2024.
//

#include <utility>


#include "StoermerVerlet.h"
#include "utils/ArrayUtils.h"

namespace md::core {
	StoermerVerlet::StoermerVerlet(ParticleContainer& particles, Force force_func, io::OutputWriter & writer):
		Integrator(particles, writer),
		force_func(std::move(force_func))
	{}


	void StoermerVerlet::simulation_step(const double dt) {

		// calculate forces
		for (auto &p : particles) {
			p.old_force = p.force;
			p.force = {0,0,0};
		}

		for (size_t i = 0; i < particles.size(); ++i) {
			auto& p1 = particles[i];
			for (size_t j = i + 1; j < particles.size(); ++j) {
				auto& p2 = particles[j];
				vec3 new_F = force_func(p1, p2);

				p2.force = p2.force - new_F;
				p1.force = p1.force + new_F;
			}
		}


		// update position
		for (auto &p : particles) {
			p.position = p.position + (dt * p.velocity) + (pow(dt, 2)/(2*p.mass) *  p.old_force);
		}

		// update velocities
		for (auto &p : particles) {
			p.velocity = p.velocity + dt/2/p.mass * (p.force + p.old_force) ;
		}
	}
}
