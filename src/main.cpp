#include <utils/Parse.h>
#include "StoermerVerlet.h"
#include "Particle.h"
#include <iostream>
#include "io/IOStrategy.h"
#include "force.hpp"

int main(const int argc, char* argv[]) {
    md::parse::Parse parser;
    auto arguments = parser.parse_args(argc, argv);
    if (!arguments) {
        return 1;
    }

    if (arguments->show_help) {
        parser.displayHelp();
        return 0;
    }
    if (arguments->delete_output) {
        return 0;
    }
    constexpr double start_time = 0;


    md::ParticleContainer particles {};
    particles.add_cuboid({0,0,0}, {40, 8, 1}, {0,0,0}, 0.1, 1.1225, 1, 2, 0);
    particles.add_cuboid({15,15,0}, {8, 8, 1}, {0,-10,0}, 0.1, 1.1225, 1, 2, 1);

    const auto writer = md::io::createWriter(arguments->output_format.value(), particles.size());

    md::Integrator::StoermerVerlet simulator(particles, md::force::lennard_jones(5, 1), *writer);
    simulator.simulate(0, 5, 0.0002, 50);

    // md::ParticleContainer particles(arguments->file);
    // simulator.simulate(start_time, arguments->end_time.value(), arguments->delta_t.value(), 200);

    std::cout << "output written. Terminating..." << std::endl;
    return 0;
}
