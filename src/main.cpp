#include <iostream>

// #include "io/IOStrategy.h"
// #include "utils/Parse.h"
// #include "core/StoermerVerlet.h"
// #include "env/Environment.h"
// #include "env/Force.h"

#include "TestScenarios.h"

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

    four_particle_reflective_velocity_test();


    // args.duration = 10;
    // args.dt = 0.00001;
    // args.num_frames = 600;
    // const double num_steps = args.duration / args.dt;
    // const int write_freq = std::max(static_cast<int> (round(num_steps / args.num_frames)), 1);
    // SPDLOG_DEBUG("Write frequency: {}", write_freq);


    // env::Environment env;
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



    // env.add_cuboid({0,0,0}, {0,0,0}, {40,8,1}, 0.1, 1.1225, 1, 2, 0);
    // env.add_cuboid({15,15,0}, {0,-10,0}, {8,8,1}, 0.1, 1.1225, 1, 2, 1);


    // env.add_particle({2.8,2.9,0}, {0,0,0}, 1, 1);
    // env.add_particle({0.1,0.15,0}, {0,0,0}, 1, 1);
    // env.add_particle({2.5,0.1,0}, {0,0,0}, 1, 1);
    // env.add_particle({0.2,2.6,0}, {0,0,0}, 1, 1);

    // env.add_particle({9,0,0}, {-5,0,0}, 1, 2);

    // env::Boundary boundary;
    // boundary.extent = {3, 3, 1};
    // boundary.origin = {0, 0, 0};
    // boundary.set_boundary_rule(env::BoundaryRule::REPULSIVE_FORCE);
    // boundary.set_boundary_rule(env::BoundaryRule::OUTFLOW, env::BoundaryNormal::FRONT);
    // boundary.set_boundary_rule(env::BoundaryRule::OUTFLOW, env::BoundaryNormal::BACK);



    // env.set_boundary(boundary);
    // env.set_force(env::LennardJones(5, 0.1));
    // env.set_grid_constant(1);

    // env.set_force(env::LennardJones(5, 1, 5));

    // env.build();
    //
    // env::Particle & particle = env[0];

    // particle.update_position({0,-1,0});
    // particle.update_grid();
    // env.apply_boundary(particle);

    // for (auto &p : env.particles()) {
    //     std::cout << "Particle: " << p.to_string() << std::endl;
    // }
    //
    // for (auto &p : env.particles()) {
    //     std::cout << "Particle: " << p.to_string() << std::endl;
    // }

    // for (auto & x : env.cells()) {
    //     std::cout << "Cell: " << x.to_string() << std::endl;
    // }

    // for (const auto & cell_pair : env.linked_cells()) {
    //     std::cout << "Cell pair: " << cell_pair.to_string() << std::endl;
    // }

    // for (auto& cell_pair : env.linked_cells()) {
    //
    //     for (auto it = cell_pair.particles().begin(); it != cell_pair.particles().end(); ++it) {
    //         auto [p1, p2] = *it;
    //          std::cout << "particle pair ids: " << p1->id << ", " << p2->id << " Positions: " << p1->position << ", " << p2->position << std::endl;
    //
    //     }
    // }



    // auto writer = args.benchmark ? nullptr : create_writer(args.output_format, args.override);
    // md::Integrator::StoermerVerlet simulator(env, std::move(writer));
    // simulator.simulate(0, args.duration, args.dt, write_freq, args.benchmark);
    //
    SPDLOG_INFO("Output written. Terminating...");
    return 0;
}
