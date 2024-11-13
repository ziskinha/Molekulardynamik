
#pragma once

#include <string>

#include "force.h"
#include "Particle.h"

namespace md::io {


	/**
	 * @brief reads a txt file with particle and particle cuboid information
	 * @param file_name
	 * @param container
	 * @param force
	 */
	void read_file_txt(const std::string& file_name, ParticleContainer& container, force::ForceFunc & force);


	// void read_file_xml(const std::string& file_name, ParticleContainer& particles);
}
