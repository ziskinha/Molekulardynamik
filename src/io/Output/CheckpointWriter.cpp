#include "CheckpointWriter.h"
#include "io/Logger/Logger.h"
#include <fstream>
#include "iostream"

constexpr const char* OUTPUT_DIR = "Checkpoint";
constexpr const char* GENERAL_HEADER = "# duration   delta_t     write_freq   cutoff_radius   output_baseName";
constexpr const char* PARTICLE_HEADER = "# origin             velocity         mass    type     old_force";
constexpr const char* FORCE_HEADER = "# name          parameter   particle_type";
constexpr const char* ENVIRONMENT_HEADER = "# (Boundary condition identification: 0: OUTFLOW, 1: PERIODIC, "
                                           "2: REPULSIVE FORCE, 3: VELOCITY REFLECTION)\n"
                                           "# boundary_origin     boundary_extent     grid_constant   grav_const    "
                                           "boundary_conds(left, right, top, bottom, front, back)";
constexpr const char* THERMOSTATS_HEADER = "# T_init     n_thermos     T_target(for no target: -1)      delta_T(for no delta_t: -1)";
constexpr const char* SPACE = "     ";

namespace md::io {

    CheckpointWriter::CheckpointWriter() : file_base_name("checkpoint") {
        if (!std::filesystem::exists(OUTPUT_DIR)) {
            std::filesystem::create_directories(OUTPUT_DIR);
        }

        if (!std::filesystem::is_empty(OUTPUT_DIR)) {
            for (const auto& entry : std::filesystem::directory_iterator(OUTPUT_DIR)) {
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
        std::filesystem::path file = std::filesystem::path(OUTPUT_DIR) / (file_base_name + std::to_string(num) + ".txt");
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

        for (size_t i = 0; i < env.size(); ++i) {
            outfile << env.operator[](i).position[0] << " " << env.operator[](i).position[1] << " " << env.operator[](i).position[2] << SPACE
                    << env.operator[](i).velocity[0] << " " << env.operator[](i).velocity[1] << " " << env.operator[](i).velocity[2] << SPACE
                    << env.operator[](i).mass << SPACE
                    << env.operator[](i).type << SPACE
                    << env.operator[](i).old_force[0] << " " << env.operator[](i).old_force[1] << " " << env.operator[](i).old_force[2] << SPACE
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