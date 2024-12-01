#include <iostream>

#include "io/IOStrategy.h"
#include "utils/Parse.h"
#include "core/StoermerVerlet.h"
#include "env/Environment.h"
#include "env/Force.h"
#include "io/Logger.h"

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

   
    const int write_freq = args.write_freq;
    SPDLOG_DEBUG("Write frequency: {}", write_freq);                            


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




    env::Boundary boundary;
    boundary.extent = {100, 100, 1};
    boundary.origin = {-10, -10, 0};
    args.env.set_boundary(boundary);
    args.env.set_grid_constant(10);
   // args.env.set_force(env::LennardJones(5, 1, 5));

    args.env.build();

    for (auto &p : args.env.particles()) {
        std::cout << "Particle: " << p.to_string() << std::endl;
    }

    for (auto & x : args.env.cells()) {
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

    Integrator::StoermerVerlet simulator(args.env, create_writer(args.output_baseName,args.output_format, args.override));
    simulator.simulate(0, args.duration, args.dt, write_freq, args.benchmark);



    // auto writer = args.benchmark ? nullptr : create_writer(args.output_format, args.override);
    // md::Integrator::StoermerVerlet simulator(env, std::move(writer));
    // simulator.simulate(0, args.duration, args.dt, write_freq, args.benchmark);
    //
    SPDLOG_INFO("Output written. Terminating...");
    return 0;
}