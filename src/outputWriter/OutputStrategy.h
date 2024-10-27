//
// Created by ziskinha on 27.10.24.
//

#include "outputWriter/OutputWriter.h"
#include "outputWriter/VTKWriter.h"
#include "outputWriter/XYZWriter.h"

namespace OutputStrategy {
    outputWriter::OutputWriter* createWriter(bool useVTK, size_t num_particles);
}
