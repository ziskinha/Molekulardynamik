#include <iostream>

#include "io/IOStrategy.h"
#include "utils/Parse.h"
#include "core/StoermerVerlet.h"
#include "env/Environment.h"
#include "env/Force.h"


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

    // args.duration = 100;
    // args.dt = 0.001;
    // args.num_frames = 1000;
    const double num_steps = args.duration / args.dt;
    const int write_freq = std::max(static_cast<int> (round(num_steps / args.num_frames)), 1);
    SPDLOG_DEBUG("Write frequency: {}", write_freq);                            


    env::Environment env;
    // io::read_file(args.file, env);
    // env.add_particle({0,1,0}, {-1,0,0}, 3.0e-6, 1);
    // env.add_particle({0,0,0}, {0,0,0}, 1, 2);


    // env.add_particle({-9,0,0}, {5,0,0}, 1, 1);
    // env.add_particle({9,0,0}, {-5,0,0}, 1, 2);
    // env::Boundary boundary;
    // boundary.extent = {20, 20, 1};
    // boundary.origin = {-10, -10, 0};
    // env.set_boundary(boundary);
    // env.set_force(env::LennardJones(5, 1, 3));



    env.add_cuboid({0,0,0}, {0,0,0}, {40,8,1}, 0.1, 1.1225, 1, 2, 0);
    env.add_cuboid({15,15,0}, {0,-10,0}, {8,8,1}, 0.1, 1.1225, 1, 2, 1);

    env::Boundary boundary;
    boundary.extent = {100, 100, 1};
    boundary.origin = {-10, -10, 0};
    env.set_boundary(boundary);
    env.set_grid_constant(10);
    env.set_force(env::LennardJones(5, 1, 5));

    env.build();

    for (auto &p : env.particles()) {
        std::cout << "Particle: " << p.to_string() << std::endl;
    }

    for (auto & x : env.cells()) {
        std::cout << "Cell: " << x.to_string() << std::endl;
    }

    // for (const auto & cell_pair : env.linked_cells()) {
    //     std::cout << "Cell pair: " << cell_pair.to_string() << std::endl;
    // }

    std::cout << "build done" << std::endl;

    // for (auto& cell_pair : env.linked_cells()) {
    //
    //     for (auto it = cell_pair.particles().begin(); it != cell_pair.particles().end(); ++it) {
    //         auto [p1, p2] = *it;
    //          std::cout << "particle pair ids: " << p1->id << ", " << p2->id << " Positions: " << p1->position << ", " << p2->position << std::endl;
    //
    //     }
    // }

    Integrator::StoermerVerlet simulator(env, create_writer(args.output_format, args.override));
    simulator.simulate(0, args.duration, args.dt, write_freq, args.benchmark);



    // auto writer = args.benchmark ? nullptr : create_writer(args.output_format, args.override);
    // md::Integrator::StoermerVerlet simulator(env, std::move(writer));
    // simulator.simulate(0, args.duration, args.dt, write_freq, args.benchmark);
    //
    SPDLOG_INFO("Output written. Terminating...");
    return 0;
}
