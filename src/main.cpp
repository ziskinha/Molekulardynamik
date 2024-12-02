#include "core/StoermerVerlet.h"
#include "io/IOStrategy.h"
#include "utils/Parse.h"
#include "io/Logger/Logger.h"

using namespace md;
int main(const int argc, char* argv[]) {
    io::Logger::initialize_logger();

    parse::ProgramArguments args;
    switch (parse_args(argc, argv, args)) {
        case parse::EXIT: return 0;
        case parse::ERROR: return -1;
        default: ;
    };

    log_arguments(args);
    args.env.build();

    Integrator::StoermerVerlet simulator(args.env, create_writer(args.output_baseName, args.output_format, args.override));
    if (!args.benchmark) {
        simulator.simulate(0, args.duration, args.dt, args.write_freq);
    } else {
        simulator.benchmark_simulate(0, args.duration, args.dt);
    }
    SPDLOG_INFO("Output written. Terminating...");

    return 0;
}