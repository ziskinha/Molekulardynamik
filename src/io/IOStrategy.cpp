#include "IOStrategy.h"

#include <iostream>
#include <string>

#include "io/Reader/FileReader.h"
#include "io/Output/VTKWriter.h"
#include "io/Output/XYZWriter.h"
#include "io/Logger/Logger.h"

namespace md::io {
    OutputWriterBase::OutputWriterBase(std::string file_name, const bool allow_delete)
        : file_name(std::move(file_name)) {
        if (!std::filesystem::exists(OUTPUT_DIR)) {
            std::filesystem::create_directories(OUTPUT_DIR);
        }

        if (allow_delete && !std::filesystem::is_empty(OUTPUT_DIR)) {
            for (const auto& entry : std::filesystem::directory_iterator(OUTPUT_DIR)) {
                if (is_regular_file(entry.path())) {
                    std::filesystem::remove(entry.path());
                }
            }
        } else if (!allow_delete && !std::filesystem::is_empty(OUTPUT_DIR)) {
            spdlog::error(
                "Output folder is not empty. Use -f to automatically override contents or empty the folder manually");
            exit(-1);
        }
    };

    bool checkFormat(const std::string& filename, const std::string& extension) {
        if (filename.length() >= extension.length()) {
            const bool format_match =
                0 == filename.compare(filename.length() - extension.length(), extension.length(), extension);
            return format_match;
        }
        return false;
    }

    std::unique_ptr<OutputWriterBase> create_writer(const std::string& outputFileBaseName,const OutputFormat output_format, bool allow_delete) {
        if (output_format == OutputFormat::VTK) {
            return std::make_unique<VTKWriter>(VTKWriter(outputFileBaseName + "_vtk", allow_delete));
        }
        return std::make_unique<XYZWriter>(XYZWriter(outputFileBaseName + "_xyz", allow_delete));
    }

    void read_file(const std::string& filename, env::Environment& environment) {
        if (checkFormat(filename, ".txt")) {
            return read_file_txt(filename, environment);
        }
        // TODO: add XML FileReader in the future
        throw std::invalid_argument("File format not supported");
    }
}  // namespace md::io
