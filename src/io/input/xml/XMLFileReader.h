#include "utils/Parse.h"

namespace md::io {
    struct ProgramArguments;
    /**
    * @brief Reads a xml file with particle, cuboid, force, environment and general information.
    * @param file_name
    * @param args
    */
    void read_file_xml(const std::string &file_name, ProgramArguments &args);
}