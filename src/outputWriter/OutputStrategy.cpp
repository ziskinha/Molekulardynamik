//
// Created by ziskinha on 27.10.24.
//

#include "OutputStrategy.h"

namespace OutputStrategy {
    outputWriter::OutputWriter* createWriter(bool useVTK, size_t num_particles) {
        if (useVTK) {
            return new outputWriter::VTKWriter("MD_vtk", num_particles);
        }
        else {
            return new outputWriter::XYZWriter("MD_xyz");
        }
    }
}