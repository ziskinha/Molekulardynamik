//
// Created by jdyma on 26/10/2024.
//

#include "StoermerVerlet.h"

#include "utils/ArrayUtils.h"

StoermerVerlet::StoermerVerlet(ParticleContainer& particles, outputWriter::OutputWriter & writer):
	SimulationAlgorithm(particles, writer)
{}


void StoermerVerlet::simulation_step(const double dt) {

	// calculate forces
	// TODO: using newtons actio = reactio halve the number of calcualtions
	for (auto &p1 : particles) {
		std::array<double, 3> new_F = {};
		for (auto &p2 : particles) {
			if (p1 == p2) continue;
			double dist  = ArrayUtils::L2Norm(p1.position - p2.position);
			double f_mag =  p1.mass * p2.mass / pow(dist, 3);
			new_F = new_F + f_mag  * (p2.position - p1.position);
		}
		p1.force = new_F;
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
