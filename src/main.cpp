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

    auto writer = io::create_writer(args.output_baseName, args.output_format, args.override);
    auto checkpoint_writer = io::create_checkpoint_writer();
    Integrator::StoermerVerlet simulator(args.env, std::move(writer), std::move(checkpoint_writer), args.thermostat);

    if (!args.benchmark) {
        simulator.simulate(0, args.duration, args.dt ,args.write_freq, args.temp_adj_freq);
    } else {
        simulator.benchmark(0, args.duration, args.dt);
    }

}

int main(const int argc, char* argv[]) {
    ws4_task2_big();

    return 0;
}

 