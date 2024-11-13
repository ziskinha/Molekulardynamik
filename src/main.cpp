#include <iostream>

#include "utils/Parse.h"
#include "StoermerVerlet.h"
#include "Particle.h"
#include "io/IOStrategy.h"
#include "force.h"

int main(const int argc, char* argv[]) {
    md::io::Logger::initialize_logger(spdlog::level::info);

    md::parse::ProgramArguments args;
    switch (parse_args(argc, argv, args)) {
    case md::parse::EXIT: return 0;
    case md::parse::ERROR: return -1;
    default: ;
    };

    log_arguments(args);
    if (args.verbose) spdlog::set_level(spdlog::level::debug);

    const double num_steps = args.duration / args.dt;
    const int write_freq = std::max(static_cast<int> (round(num_steps / args.num_frames)), 1);
    spdlog::debug("Write frequency: {}", write_freq);

    if (args.benchmark) {
        spdlog::info("benchmarking enabled. Subsequent logging messages during simulation are disabled");
        spdlog::set_level(spdlog::level::off);
    }

    md::force::ForceFunc force;
    md::ParticleContainer particles;
    md::io::read_file(args.file, particles, force);

    auto writer = args.benchmark ? nullptr : create_writer(args.output_format, args.override);
    md::Integrator::StoermerVerlet simulator(particles, force, std::move(writer));
    simulator.simulate(0, args.duration, args.dt, write_freq, args.benchmark);

    spdlog::set_level(spdlog::level::info);
    spdlog::info("Output written. Terminating...");
    return 0;
}
