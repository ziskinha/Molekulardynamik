/*
 * XYZWriter.h
 *
 *  Created on: 01.03.2010
 *      Author: eckhardw
 */

#pragma once

#include <fstream>

#include "OutputWriter.h"

namespace outputWriter {
	class XYZWriter : public OutputWriter {
	public:
		explicit XYZWriter(std::string fileName);
		~XYZWriter() override;

		void plot_particles(const ParticleContainer & container, int iteration) override;
	};
} // namespace outputWriter
