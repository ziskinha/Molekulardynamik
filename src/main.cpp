#include "core/StoermerVerlet.h"
#include "io/IOStrategy.h"
#include "utils/Parse.h"
#include "io/Logger/Logger.h"
#include "demos.h"

using namespace md;

void run_mol_sim(const int argc, char* argv[]) {
    io::Logger::initialize_logger();

    io::ProgramArguments args;
    switch (parse::parse_args(argc, argv, args)) {
        case parse::EXIT: exit(0);
        case parse::ERROR: exit(-1);
        default: ;
    };

    auto writer = create_writer(args.output_baseName, args.output_format, args.override);
    Integrator::StoermerVerlet simulator(args.env, std::move(writer));
    if (!args.benchmark) {
        simulator.simulate(0, args.duration, args.dt, args.write_freq);
    } else {
        simulator.benchmark(0, args.duration, args.dt);
    }
    SPDLOG_INFO("Output written. Terminating...");
}


int main(const int argc, char* argv[]) {
    SPDLOG_INFO("TODO: Main zurücksetzen");

    run_mol_sim(argc, argv);
    //ws4_task2_small();
    return 0;
}

