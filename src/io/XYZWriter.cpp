/*
 * XYZWriter.cpp
 *
 *  Created on: 01.03.2010
 *      Author: eckhardw
 */

#include "io/XYZWriter.h"
#include <iomanip>
#include <sstream>
#include <utility>

namespace md::io {
	XYZWriter::XYZWriter(std::string file_name): OutputWriter(std::move(file_name)) {}
	XYZWriter::~XYZWriter() = default;

	void XYZWriter::plot_particles(const core::ParticleContainer& container, int iteration) {
		std::ofstream file;
		std::stringstream strstr;
		strstr << file_name << "_" << std::setfill('0') << std::setw(4) << iteration << ".xyz";

		file.open(strstr.str().c_str());
		file << container.size() << std::endl;
		file << "Generated by MolSim. See http://openbabel.org/wiki/XYZ_(format) for "
			"file format doku."
			<< std::endl;

		for (auto& particle : container) {
			std::array<double, 3> x = particle.position;
			file << "Ar ";
			file.setf(std::ios_base::showpoint);

			for (auto& xi : x) {
				file << xi << " ";
			}

			file << std::endl;
		}

		file.close();
	}
} // namespace outputWriter