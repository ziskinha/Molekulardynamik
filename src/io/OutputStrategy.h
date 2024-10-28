#pragma once

#include <memory>

#include "Particle.h"
#include <string>
#include <utility>


/**
 * @brief Namespace containing classes and functions for output writing.
 */
namespace md::io {

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
		explicit OutputWriter(std::string file_name): file_name(std::move(file_name)) {};
		virtual ~OutputWriter() = default;

		/**
		 * @brief Outputs particle data at a given iteration.
		 * @param container The particle container to be plotted.
		 * @param iteration The current simulation iteration.
		 */
		virtual void plot_particles(const core::ParticleContainer& container, int iteration) = 0;

	protected:
		const std::string file_name;
	};


	/**
	 * @brief Factory function to create an output writer.
	 * @param useVTK A boolean flag indicating whether to use the VTK output format or XYZ.
	 * @param num_particles
	 * @return A pointer to an OutputWriter object.
	 */
	std::unique_ptr<OutputWriter> createWriter(bool useVTK, size_t num_particles);
}
