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
	std::vector<Particle> read_file_txt(const std::string& file_name);
}
