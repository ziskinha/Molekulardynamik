/*
 * FileReader.h
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#pragma once

#include "Particle.h"

namespace md::io {
	class FileReader final {
	public:
	    FileReader() = default;
	    ~FileReader() = default;

	    static std::vector<core::Particle> read_file(char* filename);
	};
}
