#include <iostream>

#include "utils/Parse.h"
#include "StoermerVerlet.h"
#include "Particle.h"
#include "io/IOStrategy.h"
#include "force.h"
#include "io/FileReader.h"

int main(const int argc, char* argv[]) {

    md::parse::ProgramArguments args;
    switch (parse_args(argc, argv, args)) {
    case md::parse::EXIT: return 0;
    case md::parse::ERROR: return -1;
    default: ;
    };

    std::cout << args;

    const double num_frames = args.duration * args.fps;
    const double num_steps = args.duration / args.dt;
    const int write_freq = std::max(static_cast<int> (round(num_steps / num_frames)), 1);

    std::cout << write_freq;

    auto writer = md::io::createWriter(args.output_format, args.delete_output_folder_contents);
    md::ParticleContainer particles(md::io::read_file(args.file));
    // particles.add_cuboid({0,0,0}, {40, 8, 1}, {0,0,0}, 0.1, 1.1225, 1, 2, 0);
    // particles.add_cuboid({15,15,0}, {8, 8, 1}, {0,-10,0}, 0.1, 1.1225, 1, 2, 1);

    // md::Integrator::StoermerVerlet simulator(particles, md::force::lennard_jones(5, 1), std::move(writer));
    md::Integrator::StoermerVerlet simulator(particles, md::force::inverse_square(), std::move(writer));

    simulator.simulate(0, args.duration, args.dt, write_freq);

    std::cout << "output written. Terminating..." << std::endl;
    return 0;
}
