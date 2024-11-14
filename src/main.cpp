#include "Particle.h"
#include "StoermerVerlet.h"
#include "force.h"
#include "io/IOStrategy.h"
#include "utils/Parse.h"

int main(const int argc, char* argv[]) {
    md::io::Logger::initialize_logger();

    md::parse::ProgramArguments args;
    switch (parse_args(argc, argv, args)) {
        case md::parse::EXIT:
            return 0;
        case md::parse::ERROR:
            return -1;
        default:;
    };

    log_arguments(args);

    const double num_steps = args.duration / args.dt;
    const int write_freq = std::max(static_cast<int>(round(num_steps / args.num_frames)), 1);
    SPDLOG_DEBUG("Write frequency: {}", write_freq);

    md::force::ForceFunc force;
    md::ParticleContainer particles;
    md::io::read_file(args.file, particles, force);

    auto writer = args.benchmark ? nullptr : create_writer(args.output_format, args.override);
    md::Integrator::StoermerVerlet simulator(particles, force, std::move(writer));
    simulator.simulate(0, args.duration, args.dt, write_freq, args.benchmark);

    SPDLOG_INFO("Output written. Terminating...");
    return 0;
}
