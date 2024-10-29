//
// Created by ziskinha on 27.10.24.
//

#include "OutputStrategy.h"
#include "VTKWriter.h"
#include "XYZWriter.h"

namespace md::io {
    std::unique_ptr<OutputWriter> createWriter(const bool output_format, const size_t num_particles) {
        if (output_format) {
            return std::make_unique<VTKWriter>(VTKWriter("MD_vtk", num_particles));
        }
        return std::make_unique<XYZWriter>(XYZWriter("MD_xyz"));
    }
}