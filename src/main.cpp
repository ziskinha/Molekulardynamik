#include <utils/parse.h>
#include "StoermerVerlet.h"
#include "Particle.h"
#include <iostream>
#include "io/OutputStrategy.h"
#include "force.hpp"

int main(const int argc, char* argv[]) {
    md::Parse::parse parser;
    auto arguments= parser.parse_args(argc,argv);
    if (!arguments) {
        return 1;
    }
    const md::Parse::parse::Parse_arguments args = *arguments;
    if (args.show_help) {
        parser.displayHelp();
        return 0;
    }
    if (args.delete_output) {
        return 0;
    }
    constexpr double start_time = 0;

    md::ParticleContainer particles(args.file);
    const auto writer = md::io::createWriter(args.output_format.value(), particles.size());
    md::Integrator::StoermerVerlet simulator(particles, md::force::inverse_square(), *writer);
    simulator.simulate(start_time, args.end_time.value(), args.delta_t.value(), 200);

    std::cout << "output written. Terminating..." << std::endl;
    return 0;
}