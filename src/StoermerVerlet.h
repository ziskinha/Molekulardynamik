#pragma once


#include "SimulationAlgorithm.h"


class StoermerVerlet : public SimulationAlgorithm{
public:
	StoermerVerlet(ParticleContainer & particles, outputWriter::VTKWriter & writer);

private:
	void simulation_step(double dt) override;
};



