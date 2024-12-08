#include "FileReader.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <locale>
#include <sstream>
#include <array>
#include <ranges>

#include "env/Environment.h"
#include "env/Boundary.h"
#include "env/Force.h"
#include "io/Logger/Logger.h"
#include "utils/Parse.h"
#include "env/Thermostat.h"

namespace md::io {
    using namespace env;
    // code from https://stackoverflow.com/questions/216823/how-to-trim-a-stdstring
    // trim from start (in place)
    void ltrim(std::string& s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
    }

    // trim from end (in place)
    void rtrim(std::string& s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
    }

    void trim(std::string& s) {
        rtrim(s);
        ltrim(s);
    }

    /// -----------------------------------------
    /// \brief Parse article information
    /// -----------------------------------------
    void parse_particle(const std::string& line, Environment& env) {
        SPDLOG_INFO("Reading Particle:    {}", line);
        std::istringstream data_stream(line);
        std::vector<double> vals;
        double num;

        while (data_stream >> num) {
            vals.push_back(num);
        }

        if (vals.size() < 7) {
            SPDLOG_ERROR("Not enough numbers in line: {}", line);
            exit(-1);
        }

        vec3 origin = {vals[0], vals[1], vals[2]};
        vec3 init_v = {vals[3], vals[4], vals[5]};
        double mass = vals[6];
        int type = 0;
        if (vals.size() == 7) {
            type = static_cast<int>(vals[7]);
        }

        SPDLOG_DEBUG(
            "Parsed Particle:\n"
            "       Origin:           [{}, {}, {}]\n"
            "       Initial Velocity: [{}, {}, {}]\n"
            "       Mass:             {}\n"
            "       Type:             {}",
            origin[0], origin[1], origin[2], init_v[0], init_v[1], init_v[2], mass, type);

        env.add_particle({origin[0], origin[1], origin[2]}, {init_v[0], init_v[1], init_v[2]}, mass, type);
    }

    /// -----------------------------------------
    /// \brief Parse cuboid information
    /// -----------------------------------------
    void parse_cuboid(const std::string& line, Environment& environment) {
        SPDLOG_DEBUG("Reading Cuboid:     {}", line);

        std::istringstream data_stream(line);
        std::vector<double> vals;
        double num;

        while (data_stream >> num) {
            vals.push_back(num);
        }

        // Minimum required values: 3 (x) + 3 (v) + 3 (#particles) + 1 (width) + 1 (mass) + 1 (thermal_v) + 1 (dim)
        if (vals.size() < 13) {
            SPDLOG_ERROR("Not enough numbers in line: {}", line);
            exit(-1);
        }

        const vec3 origin = {vals[0], vals[1], vals[2]};
        const vec3 init_v = {vals[3], vals[4], vals[5]};
        const uint3 num_particles = {static_cast<uint32_t>(vals[6]), static_cast<uint32_t>(vals[7]),
                                     static_cast<uint32_t>(vals[8])};

        const double width = vals[9];
        const double mass = vals[10];
        const double thermal_v = vals[11];
        const auto dimension = static_cast<uint32_t>(vals[12]);
        if (dimension != 2 && dimension != 3) {
            SPDLOG_ERROR("Invalid dimension parameter {}", line);
        }

        int type = 0;
        if (vals.size() == 14) {
            type = static_cast<int>(vals[13]);
        }

        SPDLOG_DEBUG(
            "Parsed Cuboid:\n"
            "       Origin:              [{}, {}, {}]\n"
            "       Initial Velocity:    [{}, {}, {}]\n"
            "       Number of Particles: [{}, {}, {}]\n"
            "       Width:               {}\n"
            "       Mass:                {}\n"
            "       Thermal Velocity:    {}\n"
            "       Dimension:           {}\n"
            "       Type:                {}",
            origin[0], origin[1], origin[2], init_v[0], init_v[1], init_v[2], num_particles[0], num_particles[1],
            num_particles[2], width, mass, thermal_v, dimension, type);

        environment.add_cuboid(origin, init_v, num_particles, thermal_v, width, mass, dimension, type);
    }

    /// -----------------------------------------
    /// \brief Parse sphere information
    /// -----------------------------------------
    void parse_sphere(const std::string& line, Environment& environment) {
        SPDLOG_DEBUG("Reading Sphere:     {}", line);

        std::istringstream data_stream(line);
        std::vector<double> vals;
        double num;

        while (data_stream >> num) {
            vals.push_back(num);
        }

        // Minimum required values: 3 (x) + 3 (v) + 1 (radius) + 1 (width) + 1 (mass) + 1 (thermal_v) + 1 (dim)
        if (vals.size() < 11) {
            SPDLOG_ERROR("Not enough numbers in line: {}", line);
            exit(-1);
        }

        const vec3 origin = {vals[0], vals[1], vals[2]};
        const vec3 init_v = {vals[3], vals[4], vals[5]};
        const uint radius = vals[6];

        const double width = vals[7];
        const double mass = vals[8];
        const double thermal_v = vals[9];
        const auto dimension = static_cast<uint32_t>(vals[10]);
        if (dimension != 2 && dimension != 3) {
            SPDLOG_ERROR("Invalid dimension parameter {}", line);
        }

        int type = 0;
        if (vals.size() == 12) {
            type = static_cast<int>(vals[11]);
        }

        SPDLOG_DEBUG(
                "Parsed Sphere:\n"
                "       Origin:              [{}, {}, {}]\n"
                "       Initial Velocity:    [{}, {}, {}]\n"
                "       Radius:              {}\n"
                "       Width:               {}\n"
                "       Mass:                {}\n"
                "       Thermal Velocity:    {}\n"
                "       Dimension:           {}\n"
                "       Type:                {}",
                origin[0], origin[1], origin[2], init_v[0], init_v[1], init_v[2], radius, width, mass, thermal_v, dimension, type);

        environment.add_sphere(origin, init_v, thermal_v, (int) radius, width, mass, dimension, type);
    }

    /// -----------------------------------------
    /// \brief Parse force information
    /// -----------------------------------------
    void parse_force(const std::string& line, ProgramArguments &args) {
        SPDLOG_DEBUG("Reading Force:     {}", line);
        std::istringstream data_stream(line);
        std::vector<double> vals;
        double num;

        std::string force_name;
        if (!(data_stream >> force_name)) {
            SPDLOG_ERROR("Could not read force name", line);
            exit(-1);
        }
        std::ranges::transform(force_name, force_name.begin(), [](const unsigned char c) { return std::tolower(c); });
        std::ranges::replace(force_name, '-', ' ');
        std::ranges::replace(force_name, '_', ' ');
        trim(force_name);
        ForceType force;

        while (data_stream >> num) {
            vals.push_back(num);
        }
        try {
            if (force_name == "lennard jones") {
                force = LennardJones(vals[0], vals[1], args.cutoff_radius);
                SPDLOG_INFO("For Particle Type {} using Lennard Jones with parameters: epsilon={}, sigma={}, cutoff_radius={}", vals[2], vals[0], vals[1], args.cutoff_radius);
            } else if (force_name == "inverse square") {
                force = InverseSquare(vals[0], vals[1]);
                SPDLOG_INFO("For Particle Type {} using inverse square force with parameter: pre_factor={}", vals[1], vals[0]);
            }
        } catch (std::out_of_range& e) {
            SPDLOG_ERROR("Parameter error in force parsing: {}. Line: {}", e.what(), line);
            exit(-1);
        }
        args.env.set_force(force, vals[2]);
    }

    /// -----------------------------------------
    /// \brief Parse environment information
    /// -----------------------------------------
    void parse_environment(const std::string& line, Environment& env) {
        SPDLOG_DEBUG("Reading Boundary:     {}", line);

        std::istringstream data_stream(line);
        std::vector<double> vals;
        double num;

        while (data_stream >> num) {
            vals.push_back(num);
        }

        // Minimum required values: 3 (origin) + 3 (extent) + 1 (grid_constant) + 6 (boundary conds)
        if (vals.size() < 13) {
            SPDLOG_ERROR("Not enough numbers in line: {}");
            exit(-1);
        }

        env::Boundary boundary;
        boundary.origin = {vals[0], vals[1], vals[2]};
        boundary.extent = {vals[3], vals[4], vals[5]};
        env.set_boundary(boundary);
        env.set_grid_constant(vals[6]);
        env.set_gravity_constant(vals[7]);

        SPDLOG_INFO("Environment - Boundary origin set at: [{}, {}, {}]", vals[0], vals[1], vals[2]);
        SPDLOG_INFO("Environment - Boundary extent set to: [{}, {}, {}]", vals[3], vals[4], vals[5]);
        SPDLOG_INFO("Environment - Grid constant set to: {}", vals[6]);
        SPDLOG_INFO("Environment - Gravity constant set to: {}", vals[7]);

        // Read boundary rules
        std::array<env::BoundaryRule, 6> rules = {OUTFLOW, PERIODIC, REPULSIVE_FORCE, VELOCITY_REFLECTION};
        // {LEFT, RIGHT, TOP, BOTTOM, FRONT, BACK}
        std::array<std::array<int, 3>, 6> normals = {{{-1, 0, 0}, {1, 0, 0}, {0, 1, 0},
                                                      {0, -1, 0}, {0, 0, 1}, {0, 0, -1}}};
        for (int i = 0; i < 6; ++i) {
            boundary.set_boundary_rule(rules[vals[8 + i]], normals[i]);
        }

        env.set_boundary(boundary);
        env.build();
    }

    /// -----------------------------------------
    /// \brief Parse thermostats information
    /// -----------------------------------------
    void parse_thermostats(const std::string& line, Environment& env) {
        SPDLOG_DEBUG("Reading Thermostats:     {}", line);

        std::istringstream data_stream(line);
        std::vector<double> vals;
        double num;

        while (data_stream >> num) {
            vals.push_back(num);
        }

        // Minimum required values: 1 (T_init) + 1 (n_thermos) + 1 (T_target) + 1 (delta_T)
        if (vals.size() < 4) {
            SPDLOG_ERROR("Not enough numbers in line: {}");
            exit(-1);
        }

        env::Thermostat thermostat(vals[0], vals[2], vals[3]);
        thermostat.set_initial_temperature(env);
        SPDLOG_INFO("Thermostat: Initial temperature: {}, n_thermostat: {}, Target temperature: {}, delta T: {}",
                    vals[0], vals[1], vals[2], vals[3]);
    }

    /// -----------------------------------------
    /// \brief Parse general information, such as time, wirteFrequency ...
    /// -----------------------------------------
    void parse_general(const std::string& line, ProgramArguments &args) {
        SPDLOG_DEBUG("Reading general information: {}", line);

        std::istringstream data_stream(line);
        std::vector<double> vals;
        double num;

        for (int i = 0; i < 4; ++i) {
            if (!(data_stream >> num)) {
                SPDLOG_ERROR("Not enough numbers in line: {}", line);
                exit(-1);
            }
            vals.push_back(num);
        }

        // Parse the next value as a string (output_basename)
        std::string basename;
        if (!(data_stream >> basename)) {
            SPDLOG_ERROR("Missing output basename in line: {}", line);
            exit(-1);
        }

        args.duration = vals[0];
        args.dt = vals[1];
        args.write_freq = vals[2];
        args.cutoff_radius = vals[3];
        args.output_baseName = basename;
    }


    void read_file_txt(const std::string& file_name, ProgramArguments &args) {
        std::ifstream infile(file_name);
        if (!infile.is_open()) {
            SPDLOG_ERROR("Failed opening file {}", file_name);
            exit(-1);
        }

        SPDLOG_INFO("Start reading file {}", file_name);

        std::string line;
        enum Section { NONE, GENERAL, PARTICLES, CUBOIDS, SPHERES, FORCE, ENVIRONMENT, THERMOSTATS} section = NONE;

        std::unordered_map<std::string, Section> sectionMap = {
                {"general:", GENERAL},
                {"particles:", PARTICLES},
                {"cuboids:", CUBOIDS},
                {"spheres:", SPHERES},
                {"force:", FORCE},
                {"environment:", ENVIRONMENT},
                {"thermostats:", THERMOSTATS}
        };

        while (std::getline(infile, line)) {
            trim(line);

            if (line.empty() || line[0] == '#') {
                continue;
            }

            bool sectionFound = false;
            for (const auto& [key, value] : sectionMap) {
                if (line.compare(0, key.size(), key) == 0) {
                    section = value;
                    sectionFound = true;
                    break;
                }
            }

            if (sectionFound) {
                continue;
            }

            if (section == GENERAL)
                parse_general(line, args);
            else if (section == PARTICLES)
                parse_particle(line, args.env);
            else if (section == CUBOIDS)
                parse_cuboid(line, args.env);
            else if (section == SPHERES)
                parse_sphere(line, args.env);
            else if (section == FORCE)
                parse_force(line, args);
            else if (section == ENVIRONMENT)
                parse_environment(line, args.env);
            else if (section == THERMOSTATS)
                parse_thermostats(line, args.env);
        }

        SPDLOG_INFO("File read successfully: {}", file_name);
    }
}  // namespace md::io
