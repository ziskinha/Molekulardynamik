#include <utils/Parse.h>
#include "StoermerVerlet.h"
#include "Particle.h"
#include <iostream>
#include "io/IOStrategy.h"
#include "force.hpp"
#include "io/FileReader.h"

int main(const int argc, char* argv[]) {
    md::parse::Parse parser;
    auto args = parser.parse_args(argc, argv);
    if (!args) {
        return 1;
    }

    if (args->show_help) {
        parser.displayHelp();
        return 0;
    }
    if (args->delete_output) {
        return 0;
    }
    constexpr double start_time = 0;

    int fps = 50;


    md::ParticleContainer particles(md::io::read_file(args->file));
    particles.add_cuboid({0,0,0}, {40, 8, 1}, {0,0,0}, 0.1, 1.1225, 1, 2, 0);
    particles.add_cuboid({15,15,0}, {8, 8, 1}, {0,-10,0}, 0.1, 1.1225, 1, 2, 1);

    auto writer = md::io::createWriter(args->output_format.value());

    md::Integrator::StoermerVerlet simulator(particles, md::force::lennard_jones(5, 1), std::move(writer));
    simulator.simulate(0, 5, 1, 10);

    // md::ParticleContainer particles(arguments->file);
    // simulator.simulate(start_time, arguments->end_time.value(), arguments->delta_t.value(), 200);

    std::cout << "output written. Terminating..." << std::endl;
    return 0;
}
