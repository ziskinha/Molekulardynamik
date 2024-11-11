/*
 * FileReader.h
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#pragma once

#include <string>
#include "Particle.h"

namespace md::io {

    /**
     * @brief Contains the information of a Cuboid.
     */
	struct CuboidInfo {
		using vec3 = std::array<double, 3>;
		vec3 x; // LL-coordinate
		vec3 v; // velocity
		std::array<int, 3> N; // Number of particles in each dimension
		double h; // width between each adjacent particle
		double mass;
		double thermal_v;
		int type;
	};

    /**
     * @brief Reads the input file, distinguishes between "particles" and "cuboids" sections and
     * parses the data in the ParticleContainer accordingly.
     *
     * @param file_name
     * @param particles
     */
	void read_file_txt(const std::string& file_name, ParticleContainer& particles);
}
