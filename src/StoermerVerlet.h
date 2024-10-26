#pragma once


#include "SimulationAlgorithm.h"


class StoermerVerlet : SimulationAlgorithm{
public:
	StoermerVerlet(ParticleContainer & particles, outputWriter::VTKWriter & writer);

private:
	void simulation_step(double dt) override;
};



