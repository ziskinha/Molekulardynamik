#include "core/StoermerVerlet.h"
#include "io/IOStrategy.h"
#include "utils/Parse.h"
#include "io/Logger/Logger.h"
#include "demos.h"
#include <iostream>

using namespace md;

void run_mol_sim(const int argc, char* argv[]) {
    io::Logger::initialize_logger();

    io::ProgramArguments args;
    switch (parse::parse_args(argc, argv, args)) {
        case parse::EXIT: exit(0);
        case parse::ERROR: exit(-1);
        default: ;
    };

    auto writer = io::create_writer(args.output_baseName, args.output_format, args.override);
    Integrator::StoermerVerlet simulator(args.env, std::move(writer));
    auto start = std::chrono::system_clock::now();
    double particle_modifications=0;
    if (!args.benchmark) {
        simulator.simulate(0, args.duration, args.dt, particle_modifications ,args.write_freq);
    } else {
        simulator.benchmark(0, args.duration, args.dt, args.temp_adj_freq);
    }
    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    SPDLOG_INFO("Execution time: {} milliseconds\n",elapsed.count());

  

    SPDLOG_INFO("Output written. Terminating...");
}


int main(const int argc, char* argv[]) {
    SPDLOG_INFO("TODO: Main zurücksetzen");

     //run_mol_sim(argc,argv);
    //ws4_task2_small();

     parse::ProgramArguments args;
    args.output_format = io::OutputFormat::VTK;
    args.benchmark = false;
    args.override = true;

    args.duration = 10;
    args.dt = 0.0001;
    args.write_freq = 100;


    env::Environment env;
    env.add_particle({2.8,2.9,0}, {4,2,0}, 1, 1);
    env.add_particle({0.1,0.15,0}, {-6,1,0}, 1, 1);
    env.add_particle({2.5,0.1,0}, {-1,1,0}, 1, 1);
    env.add_particle({0.2,2.6,0}, {6,2,0}, 1, 1);

    env::Boundary boundary;
    boundary.extent = {3, 3, 1};
    boundary.origin = {0, 0, 0};
    boundary.set_boundary_rule(env::BoundaryRule::REPULSIVE_FORCE);
    boundary.set_boundary_rule(env::BoundaryRule::OUTFLOW, env::BoundaryNormal::FRONT);
    boundary.set_boundary_rule(env::BoundaryRule::OUTFLOW, env::BoundaryNormal::BACK);
    boundary.set_boundary_force(env::Boundary::LennardJonesForce(5, 0.1));

    env.set_boundary(boundary);
    env.set_force(env::LennardJones(5, 0.1, 0.1), 1);
    env.set_grid_constant(0.1);

    env.build();

    Integrator::StoermerVerlet simulator(env, create_writer(args.output_baseName, args.output_format, args.override));
      auto start = std::chrono::system_clock::now();
    double particle_modifications=0;
    if (!args.benchmark) {
        simulator.simulate(0, args.duration, args.dt, particle_modifications ,args.write_freq);
    } else {
        simulator.benchmark(0, args.duration, args.dt);
    }
    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    SPDLOG_INFO("Execution time: {} milliseconds\n",elapsed.count());
    SPDLOG_INFO("Particles modifications: {} \n",particle_modifications);

    return 0;
}

