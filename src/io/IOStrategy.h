#pragma once

#include <memory>

#include "Particle.h"
#include <string>
#include <utility>
#include <filesystem>


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
		virtual void plot_particles(const ParticleContainer& container, int iteration) = 0;

	protected:
		const std::string file_name;
	};

    /**
     * @brief Checks if the filename ends with the specific extension.
     * @param filename The filename of the file to check.
     * @param extension The file extension to check for, e.g., ".txt" or ".xml"
     * @return true if the filename ends with the specific extension, false otherwise.
     */
    bool checkFormat(const char* filename, const std::string& extension);

	/**
	 * @brief Factory function to create an output writer.
	 * @param output_format A boolean flag indicating whether to use the VTK output format or XYZ.
	 * @param num_particles
	 * @return A pointer to an OutputWriter object.
	 */
	std::unique_ptr<OutputWriter> createWriter(bool output_format, size_t num_particles);

    /**
     * @brief Reads a file.
     * Not used yet. Preparation for the XML files.
     * @param filename
     * @return A vector of Particle objects containing the data read from the file.
     */
    std::vector<Particle> read_file(char *filename);
}
