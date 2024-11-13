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


	void parse_particle(const std::string& line, std::vector<Particle>& particle_list) {
		spdlog::debug("Reading Particle:    {}", line);
		std::istringstream data_stream(line);
		std::vector<double> vals;
		double num;

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


	void parse_cuboid(const std::string& line, ParticleContainer& container) {
		spdlog::debug("Reading Cuboid:     {}", line);

		std::istringstream data_stream(line);
		std::vector<double> vals;
		double num;

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


	void parse_force(const std::string& line, force::ForceFunc& force) {
		spdlog::debug("Reading Force:     {}", line);
		std::istringstream data_stream(line);
		std::vector<double> vals;
		double num;

		std::string force_name;
		if (!(data_stream >> force_name)) {
			spdlog::error("Could not read force name", line);
			exit(-1);
		}
		std::transform(force_name.begin(), force_name.end(), force_name.begin(),
		               [](unsigned char c) { return std::tolower(c); });
		std::replace(force_name.begin(), force_name.end(), '-', ' ');
		std::replace(force_name.begin(), force_name.end(), '_', ' ');
		trim(force_name);

		while (data_stream >> num) {
			vals.push_back(num);
		}
		try {
			if (force_name == "lennard jones") {
				force = force::lennard_jones(vals[0], vals[1]);
				spdlog::info("Using Lennard Jones with parameters: epsilon={}, sigma={}", vals[0], vals[1]);
			}
			else if (force_name == "hookes law") {
				force = force::hookes_law(vals[0], vals[1]);
				spdlog::info("Using Hookes Law with parameters: k={}, l={}", vals[0], vals[1]);
			}
			else if (force_name == "inverse square") {
				force = force::inverse_square(vals[0]);
				spdlog::info("Using inverse square force with parameter: pre_factor={}", vals[0] );
			}
		} catch (std::out_of_range & e) {
			spdlog::error("Parameter error in force parsing: {}. Line: {}", e.what(), line);
			exit(-1);
		}
	}


	void read_file_txt(const std::string& file_name, ParticleContainer& container, force::ForceFunc& force) {
		std::ifstream infile(file_name);
		if (!infile.is_open()) {
			spdlog::error("Failed opening file {}", file_name);
			exit(-1);
		}

		spdlog::info("Started reading file {}", file_name);

		std::string line;
		std::vector<Particle> particle_list;
		enum Section { NONE, PARTICLES, CUBOIDS, FORCE } section = NONE;

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
			if (line.compare(0, 8, "force:") == 0) {
				section = FORCE;
				continue;
			}

			if (section == PARTICLES) parse_particle(line, particle_list);
			else if (section == CUBOIDS) parse_cuboid(line, container);
			else if (section == FORCE) parse_force(line, force);
		}
		container.add_particles(particle_list);

		if (!force) {
			spdlog::error("No force specified in {}", file_name);
			exit(-1);
		}

		spdlog::info("File read successfully: {}", file_name);
	}
}
