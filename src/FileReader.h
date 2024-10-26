/*
 * FileReader.h
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#pragma once

#include "Particle.h"
#include "ParticleContainer.h"

#include <list>

class FileReader final {
public:
    FileReader() = default;
    ~FileReader() = default;

    static std::vector<Particle> read_file(char* filename);
};
