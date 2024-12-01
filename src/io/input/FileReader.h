
#pragma once

#include <string>

#include "env/Environment.h"

namespace md::io {

    /**
     * @brief Reads a txt file with particle, cuboid, force and environment information.
     * @param file_name
     * @param env
     */
    void read_file_txt(const std::string& file_name, env::Environment& env);

    /**
     * @brief Reads a xml file with particle, cuboid, force and environment information.
     * @param file_name
     * @param env
     */
    //void read_file_xml(const std::string& file_name, env::Environment& env);
}  // namespace md::io
