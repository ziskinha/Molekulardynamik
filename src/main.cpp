#include <iostream>

#include "utils/Parse.h"
#include "StoermerVerlet.h"
#include "Particle.h"
#include "io/IOStrategy.h"
#include "force.h"

int main(const int argc, char* argv[]) {
    md::io::Logger::initialize_logger(spdlog::level::debug);

    md::parse::ProgramArguments args;
    switch (parse_args(argc, argv, args)) {
    case md::parse::EXIT: return 0;
    case md::parse::ERROR: return -1;
    default: ;
    };

    md::parse::log_arguments(args);

    const double num_steps = args.duration / args.dt;
    const int write_freq = std::max(static_cast<int> (round(num_steps / args.num_frames)), 1);
    spdlog::info("Write frequency: {}", write_freq);

    md::ParticleContainer particles;
    md::io::read_file(args.file, particles);
    //particles.add_cuboid({0,0,0}, {0,0,0}, {40, 8, 1}, 0.1, 1.1225, 1, 2, 0);
    //particles.add_cuboid({15,15,0}, {0,-10,0}, {8, 8, 1}, 0.1, 1.1225, 1, 2, 1);

    auto writer = args.benchmark ? nullptr : createWriter(args.output_format, args.override);
    md::Integrator::StoermerVerlet simulator(particles, md::force::lennard_jones(5, 1), std::move(writer));
    simulator.simulate(0, args.duration, args.dt, write_freq, args.benchmark);

    spdlog::info("Output written. Terminating...");
    return 0;
}
