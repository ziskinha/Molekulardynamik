#include "IOStrategy.h"
#include "VTKWriter.h"
#include "XYZWriter.h"
#include "FileReader.h"
#include "io/Logger.h"
#include <iostream>
#include <string>


namespace md::io {
    OutputWriter::OutputWriter(std::string file_name, const bool allow_delete): file_name(std::move(file_name)) {
        if (!std::filesystem::exists(OUTPUT_DIR)) {
            std::filesystem::create_directories(OUTPUT_DIR);
        }

        if (allow_delete && !std::filesystem::is_empty(OUTPUT_DIR)) {
            for (const auto& entry : std::filesystem::directory_iterator(OUTPUT_DIR)) {
                if (is_regular_file(entry.path())) {
                    std::filesystem::remove(entry.path());
                }
            }
        }
        else if (!allow_delete && !std::filesystem::is_empty(OUTPUT_DIR)) {
            spdlog::error("Output folder is not empty. Use -f to automatically override contents or empty the folder manually");
            exit(-1);
        }
    };

    bool checkFormat(const std::string& filename, const std::string& extension) {
        if (filename.length() >= extension.length()) {
            bool format_match = (0 == filename.compare(filename.length() - extension.length(), extension.length(),
                                                       extension));
            return format_match;
        }
        return false;
    }

    std::unique_ptr<OutputWriter> createWriter(const OutputFormat output_format, bool allow_delete) {
        if (output_format == OutputFormat::VTK) {
            return std::make_unique<VTKWriter>(VTKWriter("MD_vtk", allow_delete));
        }
        return std::make_unique<XYZWriter>(XYZWriter("MD_xyz", allow_delete));
    }

    std::vector<Particle> read_file(const std::string& filename) {
        if (checkFormat(filename, ".txt")) {
            return read_file_txt(filename);
        }
        // TODO: add XML FileReader in the future
        throw std::invalid_argument("File format not supported");
    }
}
