#include <iostream>

#include "io/IOStrategy.h"
#include "utils/Parse.h"
#include "StoermerVerlet.h"
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

    args.duration = 1000;
    args.dt = 0.014;
    args.num_frames = 1000;
    const double num_steps = args.duration / args.dt;
    const int write_freq = std::max(static_cast<int> (round(num_steps / args.num_frames)), 1);
    SPDLOG_DEBUG("Write frequency: {}", write_freq);                            

    env::Environment env;
    io::read_file(args.file, env);
    // env.add_particle({-1,0,0}, {1,0,0}, 1, 1);
    // env.add_particle({0,1,0}, {0,0,0}, 1, 2);
    // env.add_particle({1,0,0}, {0,0,0}, 1, 3);
    // env.add_particle({1,1,0}, {0,0,0}, 1, 4);
    // env.add_particle({2,2,0}, {0,0,0}, 1, 5);
    // env.add_cuboid({-2,-2,0}, {1,1,0}, {40,40,1}, 0, 0.1, 1, 2);

    env::Boundary boundary;
    boundary.extent = {80,80,1};
    boundary.types = {
        env::Boundary::OUTFLOW,
        env::Boundary::OUTFLOW,
        env::Boundary::OUTFLOW,
        env::Boundary::OUTFLOW,
        env::Boundary::OUTFLOW,
        env::Boundary::OUTFLOW
    };
    boundary.origin = {-10,-10, 0};
    env.set_boundary(boundary);

    env.set_force(env::InverseSquare(1, 40));

    env.build();

    for (auto &p : env.particles()) {
        std::cout << "Particle: " << p.to_string() << std::endl;
    }

    for (auto & x : env.cells()) {
        std::cout << "Cell: " << x.to_string() << std::endl;
    }

    for (const auto & cell_pair : env.linked_cells()) {
        std::cout << "Cell pair: " << cell_pair.to_string() << std::endl;
    }

    std::cout << "build done" << std::endl;

    for (auto& cell_pair : env.linked_cells()) {
        // bool show = false;

        auto it = cell_pair.particles().begin();
        for (; it != cell_pair.particles().end(); ++it) {
            auto [p1, p2] = *it;
             std::cout << "particle pair ids: " << p1->id << ", " << p2->id << " Positions: " << p1->position << ", " << p2->position << std::endl;

        }
    }

    Integrator::StoermerVerlet simulator(env, create_writer(args.output_format, args.override));
    simulator.simulate(0, args.duration, args.dt, write_freq, args.benchmark);



    // auto writer = args.benchmark ? nullptr : create_writer(args.output_format, args.override);
    // md::Integrator::StoermerVerlet simulator(env, std::move(writer));
    // simulator.simulate(0, args.duration, args.dt, write_freq, args.benchmark);
    //
    SPDLOG_INFO("Output written. Terminating...");
    return 0;
}
