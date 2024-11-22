
#pragma once

#include <string>

#include "env/Environment.h"

namespace md::io {

    /**
     * @brief reads a txt file with particle and particle cuboid information
     * @param file_name
     * @param env
     */
    void read_file_txt(const std::string& file_name, env::Environment & env);

    // void read_file_xml(const std::string& file_name, ParticleContainer& particles);
}  // namespace md::io
