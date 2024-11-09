/*
 * FileReader.cpp
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#include "io/FileReader.h"
#include "io/Logger.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

namespace md::io {
	std::vector<Particle> read_file_txt(const std::string& file_name) {
		std::vector<Particle> particles = {};
		int num_particles = 0;
		std::array<double, 3> x{};
		std::array<double, 3> v{};
		double m;

		std::ifstream input_file(file_name);
		if (!input_file.is_open()) {
			spdlog::error("Error: could not open file {}", file_name);
			exit(-1);
		}

        spdlog::info("File {} opened successfully", file_name);
        spdlog::info("Start reading file");

		std::string tmp_string;
		getline(input_file, tmp_string);
        spdlog::info("Read line: {}", tmp_string);

		while (tmp_string.empty() or tmp_string[0] == '#') {
			getline(input_file, tmp_string);
            spdlog::info("Read line: {}", tmp_string);
		}

		std::istringstream num_stream(tmp_string);
		num_stream >> num_particles;
		particles.reserve(num_particles);

        spdlog::info("Reading {} Particles", num_particles);
		getline(input_file, tmp_string);
        spdlog::info("Read line: {}", tmp_string);

		for (int i = 0; i < num_particles; i++) {
			std::istringstream data_stream(tmp_string);

			for (auto& xj : x) {
				data_stream >> xj;
			}
			for (auto& vj : v) {
				data_stream >> vj;
			}
			if (data_stream.eof()) {
                spdlog::error("Error reading file: eof reached unexpectedly reading from line {}", i);
				exit(-1);
			}
			data_stream >> m;
			particles.emplace_back(x, v, m);

			getline(input_file, tmp_string);
            spdlog::info("Read line: {}", tmp_string);
		}

        spdlog::info("Completed reading file {}", file_name);

		return particles;
	}
}

