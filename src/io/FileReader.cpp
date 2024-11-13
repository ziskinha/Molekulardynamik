/*
 * FileReader.cpp
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#include "io/FileReader.h"

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <locale>
#include "io/Logger.h"

namespace md::io {
	// code from https://stackoverflow.com/questions/216823/how-to-trim-a-stdstring
	// trim from start (in place)
	void ltrim(std::string& s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
			return !std::isspace(ch);
		}));
	}
	// trim from end (in place)
	void rtrim(std::string& s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
			return !std::isspace(ch);
		}).base(), s.end());
	}
	void trim(std::string& s) {
		rtrim(s);
		ltrim(s);
	}


	void read_file_txt(const std::string& file_name, ParticleContainer & container) {
		std::ifstream infile(file_name);
		if (!infile.is_open()) {
			spdlog::error("Failed opening file {}", file_name);
			exit(-1);
		}

        spdlog::info("Started reading file {}", file_name);

		std::string line;
		std::vector<Particle> particle_list;
		enum Section { NONE, PARTICLES, CUBOIDS } section = NONE;

		while (std::getline(infile, line)) {
			trim(line);

			if (line.empty() || line[0] == '#') {
				continue;
			}
			if (line.compare(0, 10, "particles:") == 0) {
				section = PARTICLES;
				continue;
			}
			if (line.compare(0, 8, "cuboids:") == 0) {
				section = CUBOIDS;
				continue;
			}

			std::vector<double> vals;
			double num;

			if (section == PARTICLES) {
				spdlog::debug("Reading Particle:    {}", line);
				std::istringstream data_stream(line);

				while (data_stream >> num) {
					vals.push_back(num);
				}

				if (vals.size() < 7) {
					spdlog::error("Not enough numbers in line: {}");
					exit(-1);
				}


				vec3 origin = {vals[0], vals[1], vals[2]};
				vec3 init_v = {vals[3], vals[4], vals[5]};
				double mass = vals[6];
				int type = 0;
				if (vals.size() == 7) {
					type = static_cast<int>(vals[7]);
				}

				particle_list.emplace_back(origin, init_v, mass, type);
			}
			else if (section == CUBOIDS) {
				spdlog::debug("Reading Cuboid:     {}", line);

				std::istringstream data_stream(line);

				while (data_stream >> num) {
					vals.push_back(num);
				}

				// Minimum required values: 3 (origin) + 3 (velocities) + 3 (num_particles) + 1 (width) + 1 (mass) + 1 (thermal_v) + 1 (dimension)
				if (vals.size() < 13) {
					spdlog::error("Not enough numbers in line: {}", line);
					exit(-1);
				}

				vec3 origin = {vals[0], vals[1], vals[2]};
				vec3 init_v = {vals[3], vals[4], vals[5]};
				std::array<uint32_t, 3> num_particles = {
					static_cast<uint32_t>(vals[6]),
					static_cast<uint32_t>(vals[7]),
					static_cast<uint32_t>(vals[8])
				};

				double width = vals[9];
				double mass = vals[10];
				double thermal_v = vals[11];
				uint32_t dimension = static_cast<uint32_t>(vals[12]);
				if (dimension != 2 && dimension != 3) {
					spdlog::error("Invalid dimension parameter {}", line);
				}

				int type = 0;
				if (vals.size() == 14) {
					type = static_cast<int>(vals[13]);
				}

				spdlog::debug("Parsed Cuboid:\n"
                             "       Origin: [{}, {}, {}]\n"
                             "       Initial Velocity: [{}, {}, {}]\n"
                             "       Number of Particles: [{}, {}, {}]\n"
                             "       Width: {}\n"
                             "       Mass: {}\n"
                             "       Thermal Velocity: {}\n"
                             "       Dimension: {}\n"
                             "       Type: {}",
                             origin[0], origin[1], origin[2],
                             init_v[0], init_v[1], init_v[2],
                             num_particles[0], num_particles[1], num_particles[2],
                             width,
                             mass,
                             thermal_v,
                             dimension,
                             type);

				container.add_cuboid(origin, init_v, num_particles, thermal_v, width, mass, dimension, type);
			}
		}
		container.add_particles(particle_list);
        spdlog::info("File read successfully: {}", file_name);
	}
}
