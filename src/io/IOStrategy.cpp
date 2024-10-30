//
// Created by ziskinha on 27.10.24.
//

#include "IOStrategy.h"
#include "VTKWriter.h"
#include "XYZWriter.h"
#include "FileReader.h"
#include <iostream>
#include <string>



namespace md::io {
    bool checkFormat(const char* filename, const std::string& extension) {
        std::string file_str(filename);
        if(file_str.length() >= extension.length()) {
            bool format_match = (0 == file_str.compare(file_str.length() - extension.length(), extension.length(), extension));
            return format_match;
        }
        return false;
    }

    std::unique_ptr<OutputWriter> createWriter(const bool output_format, const size_t num_particles) {
        if (output_format) {
            return std::make_unique<VTKWriter>(VTKWriter("MD_vtk", num_particles));
        }
        return std::make_unique<XYZWriter>(XYZWriter("MD_xyz"));
    }

    std::vector<Particle> read_file(char *filename) {
        if (checkFormat(filename, ".txt")) {
            return FileReader::read_file(filename);
        }
        // TODO: add XML FileReader in the future
        return FileReader::read_file(filename);
    }
}