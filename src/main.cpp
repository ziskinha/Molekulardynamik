#include <iostream>

#include "core/StoermerVerlet.h"
#include "env/Environment.h"
#include "env/Force.h"
#include "io/IOStrategy.h"
#include "utils/Parse.h"

using namespace md;
int main(const int argc, char* argv[]) {
    io::Logger::initialize_logger();

    parse::ProgramArguments args;
    switch (parse_args(argc, argv, args)) {
        case parse::EXIT:
            return 0;
        case parse::ERROR:
            return -1;
        default:;
    };

    log_arguments(args);

    const double num_steps = args.duration / args.dt;
    const int write_freq = std::max(static_cast<int>(round(num_steps / args.num_frames)), 1);
    SPDLOG_DEBUG("Write frequency: {}", write_freq);

    env::Environment env;
    md::io::read_file(args.file, env);
    env.build();

    auto writer = args.benchmark ? nullptr : create_writer(args.output_format, args.override);
    Integrator::StoermerVerlet simulator(env, create_writer(args.output_format, args.override));
    if (!args.benchmark) {
        simulator.simulate(0, args.duration, args.dt, write_freq);
    } else {
        simulator.benchmark_simulate(0, args.duration, args.dt, args.file);
    }

    SPDLOG_INFO("Output written. Terminating...");
    return 0;
}
