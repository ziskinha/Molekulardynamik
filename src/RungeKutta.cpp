//
// Created by jdyma on 28/10/2024.
//

#include "RungeKutta.h"
#include "utils/ArrayUtils.h"

namespace md::Integrator {
	RungeKutta::RungeKutta(ParticleContainer& particles, force::ForceFunc force_func,
	                       std::unique_ptr<io::OutputWriterBase> writer):
		IntegratorBase(particles, std::move(writer)),
		force_func(std::move(force_func)) {}


	void RungeKutta::simulation_step(const double dt) {
		// Store original positions and velocities
		std::vector<vec3> position0(particles.size());
		std::vector<vec3> velocity0(particles.size());

		std::vector<vec3> k1(particles.size()), k2(particles.size()), k3(particles.size()), k4(particles.size());
		std::vector<vec3> l1(particles.size()), l2(particles.size()), l3(particles.size()), l4(particles.size());

		// k1 and l1
		for (size_t i = 0; i < particles.size(); ++i) {
			position0[i] = particles[i].position;
			velocity0[i] = particles[i].velocity;
		}
		compute_forces();
		for (size_t i = 0; i < particles.size(); ++i) {
			k1[i] = dt * particles[i].velocity;
			l1[i] = dt * (1.0 / particles[i].mass) * particles[i].force;
		}

		// k2 and l2
		for (size_t i = 0; i < particles.size(); i++) {
			particles[i].position = position0[i] + 0.5 * k1[i];
			particles[i].velocity = velocity0[i] + 0.5 * l1[i];
		}
		compute_forces();
		for (size_t i = 0; i < particles.size(); i++) {
			k2[i] = dt * particles[i].velocity;
			l2[i] = dt * (1.0 / particles[i].mass) * particles[i].force;
		}

		// k3 and l3
		for (size_t i = 0; i < particles.size(); i++) {
			particles[i].position = position0[i] + 0.5 * k2[i];
			particles[i].velocity = velocity0[i] + 0.5 * l2[i];
		}
		compute_forces();
		for (size_t i = 0; i < particles.size(); i++) {
			k3[i] = dt * particles[i].velocity;
			l3[i] = dt * (1.0 / particles[i].mass) * particles[i].force;
		}

		// k4 and l4
		for (size_t i = 0; i < particles.size(); i++) {
			particles[i].position = position0[i] + k3[i];
			particles[i].velocity = velocity0[i] + l3[i];
		}
		compute_forces();
		for (size_t i = 0; i < particles.size(); i++) {
			k4[i] = dt * particles[i].velocity;
			l4[i] = dt * (1.0 / particles[i].mass) * particles[i].force;
		}

		// Update positions and velocities
		for (size_t i = 0; i < particles.size(); i++) {
			particles[i].position = position0[i] + 1.0 / 6 * (k1[i] + 2.0 * k2[i] + 2.0 * k3[i] + k4[i]);
			particles[i].velocity = velocity0[i] + 1.0 / 6 * (l1[i] + 2.0 * l2[i] + 2.0 * l3[i] + l4[i]);
		}
	}

	void RungeKutta::compute_forces() const {
		// Reset forces
		for (auto& p : particles) {
			p.force = {0.0, 0.0, 0.0};
		}

		for (size_t i = 0; i < particles.size(); i++) {
			for (size_t j = i + 1; j < particles.size(); j++) {
				vec3 force = force_func(particles[i], particles[j]);
				particles[i].force = particles[i].force - force;
				particles[j].force = particles[j].force + force;
			}
		}
	}
}
