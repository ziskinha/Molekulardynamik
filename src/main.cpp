#include "utils/Parse.h"
#include "StoermerVerlet.h"
#include "Environment.h"
#include "io/IOStrategy.h"
#include "Force.h"

#include <iostream>

int main(const int argc, char* argv[]) {
    md::io::Logger::initialize_logger();

    md::parse::ProgramArguments args;
    switch (parse_args(argc, argv, args)) {
    case md::parse::EXIT: return 0;
    case md::parse::ERROR: return -1;
    default: ;
    };

    log_arguments(args);

    args.duration = 8;
    args.dt = 0.005;
    args.num_frames = 100;
    const double num_steps = args.duration / args.dt;
    const int write_freq = std::max(static_cast<int> (round(num_steps / args.num_frames)), 1);
    SPDLOG_DEBUG("Write frequency: {}", write_freq);                            

    md::Environment env;
    // md::io::read_file(args.file, env);
    // env.add_particle({-1,0,0}, {1,0,0}, 1, 1);
    // env.add_particle({0,1,0}, {0,0,0}, 1, 2);
    // env.add_particle({1,0,0}, {0,0,0}, 1, 3);
    // env.add_particle({1,1,0}, {0,0,0}, 1, 4);
    // env.add_particle({2,2,0}, {0,0,0}, 1, 5);
    env.add_cuboid({-2,-2,0}, {1,1,0}, {40,40,1}, 0, 0.1, 1, 2);

    md::Boundary boundary;
    boundary.extent = {2,2,1};
    env.set_boundary(boundary);
    env.set_grid_constant(1.0);
    env.set_force(md::force::no_force());

    env.build();

    // for (auto & p : env.particles()) {
    //     p.update_position(p.position + vec3{1,0,0});
    // }
    // for (auto & p : env.particles()) {
    //     p.update_position(p.position + vec3{0,1,0});
    // }
    // for (auto & p : env.particles()) {
    //     p.update_position(p.position + vec3{-1,0,0});
    // }
    // for (auto & p : env.particles()) {
    //     p.update_position(p.position + vec3{0,-1,0});
    // }
    //
    // for (auto & p : env.particles()) {
    //     p.update_position(p.position + vec3{0,-10,0});
    // }
    //
    // for (auto & p : env.particles()) {
    //     p.update_position(p.position + vec3{0,10,0});
    // }

    md::Integrator::StoermerVerlet simulator(env, create_writer(args.output_format, args.override));
    simulator.simulate(0, args.duration, args.dt, write_freq, args.benchmark);



    // auto writer = args.benchmark ? nullptr : create_writer(args.output_format, args.override);
    // md::Integrator::StoermerVerlet simulator(env, std::move(writer));
    // simulator.simulate(0, args.duration, args.dt, write_freq, args.benchmark);
    //
    SPDLOG_INFO("Output written. Terminating...");
    return 0;
}
