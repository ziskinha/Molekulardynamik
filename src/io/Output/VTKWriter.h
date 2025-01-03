/*
 * VTKWriter.h
 *
 *  Created on: 01.03.2010
 *      Author: eckhardw
 */

#pragma once

#include "io/IOStrategy.h"
#include "env/Environment.h"
#include "io/Output/VTK unstructured/vtk-unstructured.h"

namespace md::io {
    /**
     * @brief This class implements the functionality to generate vtk output from articles.
     */
    class VTKWriter : public OutputWriterBase {
       public:
        /**
         * @param filename the base name of the file to be written.
         */
        explicit VTKWriter(std::string file_base_name, bool allow_delete);
        ~VTKWriter() override;

        void plot_particles(const env::Environment& environment, int iteration) override;

       private:
        /**
         * @brief set up internal data structures and prepare to plot a particle.
         * @param numParticles
         */
        void initializeOutput(int numParticles);

        /**
         * @brief plot type, mass, position, velocity and force of a particle.
         *
         * @note: initializeOutput() must have been called before.
         */
        void plotParticle(const env::Particle& p) const;

        /**
         * @brief writes the final output file.
         *
         * @param iteration the number of the current iteration, which is used to generate an unique filename
         */
        void writeFile(unsigned int iteration) const;
        VTKFile_t* vtkFile{};
    };
}  // namespace md::io
