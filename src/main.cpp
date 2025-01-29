#include "core/IntegratorFactory.h"
#include "io/IOStrategy.h"
#include "utils/Parse.h"
#include "io/Logger/Logger.h"
#include "demos.h"
using namespace md;

int main(const int argc, char* argv[]) {
    io::Logger::initialize_logger();

    io::ProgramArguments args;
    switch (parse::parse_args(argc, argv, args)) {
        case parse::EXIT:
            exit(0);
        case parse::ERROR:
            exit(-1);
        default:;
    };

    std::unique_ptr<md::core::Statistics> stats = std::make_unique<md::core::NanoFlowStatistics>(10000, 50);
    auto simulator = md::Integrator::create_simulator(args,std::move(stats));

    if (!args.benchmark) {
        simulator->simulate(0, args.duration, args.dt, args.write_freq, args.temp_adj_freq);
    } else {
        simulator->benchmark(0, args.duration, args.dt);
    }
    return 0;
}
