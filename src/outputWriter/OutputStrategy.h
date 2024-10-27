//
// Created by ziskinha on 27.10.24.
//

#include "outputWriter/OutputWriter.h"
#include "outputWriter/VTKWriter.h"
#include "outputWriter/XYZWriter.h"

/**
 * @brief Namespace for defining different output strategies in the simulaiton.
 */
namespace OutputStrategy {
    /**
     * @brief Factory function to create an output writer.
     * @param useVTK A boolean flag indicating whether to use the VTK output format or XYZ.
     * @param num_particles
     * @return A pointer to an OutputWriter object.
     */
    outputWriter::OutputWriter* createWriter(bool useVTK, size_t num_particles);
}
