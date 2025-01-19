
#pragma once

#include <string>

namespace md::io {
    struct ProgramArguments;
    /**
     * @brief Reads a txt file with particle, cuboid, force and environment information.
     * @param file_name
     * @param env
     */
    void read_file_txt(const std::string& file_name, ProgramArguments &args);
}  // namespace md::io
