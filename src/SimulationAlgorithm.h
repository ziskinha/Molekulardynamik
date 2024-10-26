

#pragma once
#include "ParticleContainer.h"
#include "outputWriter/VTKWriter.h"


class SimulationAlgorithm {
public:
	SimulationAlgorithm(ParticleContainer & particles, outputWriter::VTKWriter & writer);
	virtual ~SimulationAlgorithm() = default;

	void simulate(const double start_time, const double end_time, const double dt, unsigned int write_freq = 10);

protected:
	virtual void simulation_step(double dt) = 0;
	ParticleContainer & particles;
private:
	outputWriter::VTKWriter writer; // TODO: replace with abstract superclass meant for implementing writing operations
};


