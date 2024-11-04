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
    bool checkFormat(const std::string & filename, const std::string& extension) {
        if(filename.length() >= extension.length()) {
            bool format_match = (0 == filename.compare(filename.length() - extension.length(), extension.length(), extension));
            return format_match;
        }
        return false;
    }

    std::unique_ptr<OutputWriter> createWriter(const bool output_format) {
        if (output_format) {
            return std::make_unique<VTKWriter>(VTKWriter("MD_vtk"));
        }
        return std::make_unique<XYZWriter>(XYZWriter("MD_xyz"));
    }

    std::vector<Particle> read_file(const std::string & filename) {
        if (checkFormat(filename, ".txt")) {
            return read_file_txt(filename);
        }
        // TODO: add XML FileReader in the future
        throw std::invalid_argument("File format not supported");
    }
}