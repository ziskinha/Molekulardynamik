#pragma once


#include "SimulationAlgorithm.h"


class StoermerVerlet : public SimulationAlgorithm{
public:
	StoermerVerlet(ParticleContainer & particles, outputWriter::OutputWriter & writer);

private:
	void simulation_step(double dt) override;
};



