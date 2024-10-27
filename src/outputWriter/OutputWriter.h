#pragma once

#include "ParticleContainer.h"
#include <string>
#include <utility>

/**
 * @brief Namespace containing classes and functions for output writing.
 */
namespace outputWriter {
    /**
     * @brief Abstract base class for output writers.
     * OutputWriter is the superclass for VTKWriter and XYZWriter.
     */
	class OutputWriter {
    public:
        /**
         * @brief Constructs an OutputWriter with a specified file name.
         * @param file_name The name of the output file.
         */
		explicit OutputWriter(std::string  file_name): file_name(std::move(file_name)) {};
		virtual ~OutputWriter() = default;

        /**
         * @brief Outputs particle data at a given iteration.
         * @param container The particle container to be plotted.
         * @param iteration The current simulation iteration.
         */
		virtual void plot_particles(const ParticleContainer & container, int iteration) = 0;
	protected:
		const std::string file_name;
	};
}



