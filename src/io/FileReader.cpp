/*
 * FileReader.cpp
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#include "io/FileReader.h"

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
			std::cout << "Error: could not open file " << file_name << std::endl;
			exit(-1);
		}

		std::string tmp_string;
		getline(input_file, tmp_string);
		std::cout << "Read line: " << tmp_string << std::endl;

		while (tmp_string.empty() or tmp_string[0] == '#') {
			getline(input_file, tmp_string);
			std::cout << "Read line: " << tmp_string << std::endl;
		}

		std::istringstream num_stream(tmp_string);
		num_stream >> num_particles;
		particles.reserve(num_particles);

		std::cout << "Reading " << num_particles << "." << std::endl;
		getline(input_file, tmp_string);
		std::cout << "Read line: " << tmp_string << std::endl;

		for (int i = 0; i < num_particles; i++) {
			std::istringstream data_stream(tmp_string);

			for (auto& xj : x) {
				data_stream >> xj;
			}
			for (auto& vj : v) {
				data_stream >> vj;
			}
			if (data_stream.eof()) {
				std::cout
					<< "Error reading file: eof reached unexpectedly reading from line "
					<< i << std::endl;
				exit(-1);
			}
			data_stream >> m;
			particles.emplace_back(x, v, m);

			getline(input_file, tmp_string);
			std::cout << "Read line: " << tmp_string << std::endl;
		}

		return particles;
	}
}

