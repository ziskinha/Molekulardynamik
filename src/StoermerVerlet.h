#pragma once


#include <functional>
#include "SimulationAlgorithm.h"
#include "utils/ArrayUtils.h"


inline vec3 inverse_square_force(const Particle& p1, const Particle& p2) {
	double dist  = ArrayUtils::L2Norm(p1.position - p2.position);
	double f_mag =  p1.mass * p2.mass / pow(dist, 3);
	return f_mag  * (p2.position - p1.position);
}


class StoermerVerlet final : public SimulationAlgorithm{
public:
	using Force = std::function<vec3(const Particle&, const Particle&)>;

	StoermerVerlet(ParticleContainer & particles, Force force_func,outputWriter::OutputWriter & writer);

private:
	void simulation_step(double dt) override;
	Force force_func;
};



