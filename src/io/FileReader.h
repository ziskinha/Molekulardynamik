
#pragma once

#include <string>
#include "Particle.h"

namespace md::io {


	/**
	 * @brief reads a txt file with particle and particle cuboid information
	 * @param file_name
	 * @param container
	 */
	void read_file_txt(const std::string& file_name, ParticleContainer& container);


	// void read_file_xml(const std::string& file_name, ParticleContainer& particles);
}
