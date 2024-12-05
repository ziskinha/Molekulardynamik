#include "core/StoermerVerlet.h"
#include "io/IOStrategy.h"
#include "utils/Parse.h"
#include "io/Logger/Logger.h"

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

    args.duration = 100;
    args.dt = 0.0001;
    args.write_freq = 1000;

    env::Environment env;
    env.set_dimension(env::TwoD);
    env.add_particle({2.8,2.9,0}, {0.1,0,0}, 1, 1);
    env.add_particle({0.1,0.15,0}, {0,0.1,0}, 1, 1);
    env.add_particle({2.5,0.1,0}, {-0.1,0,0}, 1, 1);
    env.add_particle({0.2,2.6,0}, {0,-0.1,0}, 1, 1);

    env::Boundary boundary;
    boundary.extent = {3, 3, 1};
    boundary.origin = {0, 0, 0};
    boundary.set_boundary_rule(env::BoundaryRule::VELOCITY_REFLECTION);
    boundary.set_boundary_rule(env::BoundaryRule::OUTFLOW, env::BoundaryNormal::FRONT);
    boundary.set_boundary_rule(env::BoundaryRule::OUTFLOW, env::BoundaryNormal::BACK);

    env.set_boundary(boundary);
    env.set_force(env::NoForce());
    env.set_grid_constant(0.5);

    env.build();

    env::Thermostat thermostat(0.01,20,0.02);
    // thermostat.set_initial_temperature(env);

    auto writer = create_writer(args.output_baseName, args.output_format, args.override);
    Integrator::StoermerVerlet simulator(env, std::move(writer), thermostat);
    simulator.simulate(0, args.duration, args.dt, args.write_freq, args.write_freq);

    return 0;
}

