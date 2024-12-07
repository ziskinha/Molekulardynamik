#include "core/StoermerVerlet.h"
#include "io/IOStrategy.h"
#include "utils/Parse.h"
#include "io/Logger/Logger.h"
#include "iostream"

using namespace md;

void run_mol_sim(const int argc, char* argv[]) {
    io::Logger::initialize_logger();

    parse::ProgramArguments args;
    switch (parse_args(argc, argv, args)) {
        case parse::EXIT: exit(0);
        case parse::ERROR: exit(-1);
        default: ;
    };

    log_arguments(args);
    args.env.build();

    auto writer = create_writer(args.output_baseName, args.output_format, args.override);
    Integrator::StoermerVerlet simulator(args.env, std::move(writer));
    if (!args.benchmark) {
        simulator.simulate(0, args.duration, args.dt, args.write_freq);
    } else {
        simulator.benchmark_simulate(0, args.duration, args.dt);
    }
    SPDLOG_INFO("Output written. Terminating...");
}


int main(const int argc, char* argv[]) {
    // run_mol_sim(argc, argv);

    parse::ProgramArguments args;
    args.output_format = io::OutputFormat::VTK;
    args.benchmark = false;
    args.override = true;
    args.output_baseName = "output";

    args.duration = 20;
    args.dt = 0.0001;
    args.write_freq = 1000;

    env::Environment env;
    env.set_dimension(env::TWO);
    env.add_particle({0.3,0.3,0}, {0,0,0}, 1, 1);
    env.add_particle({2.7,2.7,0}, {0,0,0}, 200000, 1);
    env.add_particle({0.3,2.7,0}, {0,0,0}, 1, 1);
    env.add_particle({2.7,0.3,0}, {0,0,0}, 1, 1);

    env::Boundary boundary;
    boundary.extent = {3, 3, 1};
    boundary.origin = {0, 0, 0};
    boundary.set_boundary_rule(env::BoundaryRule::OUTFLOW);
    boundary.set_boundary_rule(env::BoundaryRule::PERIODIC, env::BoundaryNormal::LEFT);
    boundary.set_boundary_rule(env::BoundaryRule::PERIODIC, env::BoundaryNormal::RIGHT);
    boundary.set_boundary_rule(env::BoundaryRule::PERIODIC, env::BoundaryNormal::TOP);
    boundary.set_boundary_rule(env::BoundaryRule::PERIODIC, env::BoundaryNormal::BOTTOM);

    env.set_boundary(boundary);
    env.set_force(env::LennardJones(5, 0.2, 1));
    env.set_grid_constant(1);

    env.build();

    for (auto & pair : env.linked_cells()) {
        std::cout << pair.to_string() << std::endl;
    }

    // env::Thermostat thermostat(0.01,20,0.02);
    // thermostat.set_initial_temperature(env);

    auto writer = create_writer(args.output_baseName, args.output_format, args.override);
    Integrator::StoermerVerlet simulator(env, std::move(writer));
    simulator.simulate(0, args.duration, args.dt, args.write_freq, args.write_freq);

    return 0;
}

