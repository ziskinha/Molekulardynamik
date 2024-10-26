#pragma once

#include "ParticleContainer.h"
#include <string>
#include <utility>


namespace outputWriter {
	class OutputWriter {
	public:
		explicit OutputWriter(std::string  file_name): file_name(std::move(file_name)) {};
		virtual ~OutputWriter() = default;
		virtual void plot_particles(const ParticleContainer & container, int iteration) = 0;
	protected:
		const std::string file_name;
	};
}



