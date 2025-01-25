#include "CheckpointWriter.h"
#include "io/Logger/Logger.h"
#include <fstream>
#include "iostream"

constexpr const char* GENERAL_HEADER = "# duration   delta_t     write_freq   cutoff_radius   parallel_strategy  output_baseName";
constexpr const char* PARTICLE_HEADER = "# origin             velocity         mass    type     old_force";
constexpr const char* FORCE_HEADER = "# lennard_jones/inverse_square  parameter   particle_type\n"
                                     "# gravity       direction    strength\n"
                                     "# pull_force    direction    strength   MarkBox_bottom_left_corner  MarkBox_top_right_corner  start_t end_t  const_acc(default: false)";
constexpr const char* ENVIRONMENT_HEADER = "# (Boundary condition identification: 0: OUTFLOW, 1: PERIODIC, "
                                           "2: REPULSIVE FORCE, 3: VELOCITY REFLECTION)\n"
                                           "# boundary_origin     boundary_extent     grid_constant    "
                                           "boundary_conds(left, right, top, bottom, front, back)";
constexpr const char* THERMOSTATS_HEADER = "# T_init     n_thermos     T_target(for no target: -1)      delta_T(for no delta_t: -1)";
constexpr const char* SPACE = "     ";

namespace md::io {

    CheckpointWriter::CheckpointWriter(std::string file_base_name, std::string output_dir)
        : file_base_name(file_base_name), output_dir(output_dir) {
        if (!std::filesystem::exists(output_dir)) {
            std::filesystem::create_directories(output_dir);
        }

        if (!std::filesystem::is_empty(output_dir)) {
            for (const auto& entry : std::filesystem::directory_iterator(output_dir)) {
                if (is_regular_file(entry.path())) {
                    std::filesystem::remove(entry.path());
                }
            }
        }
    }

    void section(const char* HEADER, const std::string& section, std::ofstream& outfile) {
        outfile << section << std::endl;
        outfile << HEADER << std::endl;
        outfile << "# -- Please provide the missing values or delete -- #\n" << std::endl;
    }

    void CheckpointWriter::write_checkpoint_file(env::Environment &env, size_t num) {
        std::filesystem::path file = std::filesystem::path(output_dir) / (file_base_name + std::to_string(num) + ".txt");
        std::ofstream outfile(file);

        if (!outfile.is_open()) {
            SPDLOG_ERROR("Failed opening checkpoint file {}", file.string());
            exit(-1);
        }

        SPDLOG_DEBUG("Start creating checkpoint file {}", file.string());

        outfile << "# for file format info please take a look at /input/others/txt_file_description.txt\n" << std::endl;

        //General Section
        section(GENERAL_HEADER, "general:", outfile);

        // Particle Section
        outfile << "particles:" << std::endl;
        outfile << PARTICLE_HEADER << std::endl;

        for (auto & particle : env.particles(env::GridCell::INSIDE, env::Particle::ALIVE)) {
            outfile << particle.position[0] << " " << particle.position[1] << " " << particle.position[2] << SPACE
                    << particle.velocity[0] << " " << particle.velocity[1] << " " << particle.velocity[2] << SPACE
                    << particle.mass << SPACE
                    << particle.type << SPACE
                    << particle.old_force[0] << " " << particle.old_force[1] << " " << particle.old_force[2]
                    << std::endl;
        }
        outfile << " " << std::endl;

        //Other sections
        section(FORCE_HEADER, "force:", outfile);
        section(ENVIRONMENT_HEADER, "environment:", outfile);
        section(THERMOSTATS_HEADER, "thermostats:", outfile);

        outfile.close();
        SPDLOG_DEBUG("Successfully created checkpoint file {}", file.string());
    }
} // namespace md::io