#include "core/StoermerVerlet.h"
#include "io/IOStrategy.h"
#include "utils/Parse.h"
#include "io/Logger/Logger.h"
#include "demos.h"

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
        simulator.benchmark(0, args.duration, args.dt);
    }
    SPDLOG_INFO("Output written. Terminating...");
}


int main(const int argc, char* argv[]) {
    parse::ProgramArguments args;
    args.output_format = io::OutputFormat::VTK;
    args.benchmark = false;
    args.override = true;
    args.output_baseName = "output";

    args.duration = 50;
    args.dt = 0.0005;
    args.write_freq = 100;

    env::Environment env;
    env.add_cuboid({0.6,2,0}, {0,0,0}, {250,20,1}, 0, 1.2, 1, 2, 0);
    env.add_cuboid({0.6,27,0}, {0,0,0}, {250,20,1}, 0, 1.2, 2, 2, 1);

    env::Boundary boundary;
    boundary.extent = {300, 54, 1};
    boundary.origin = {0, 0, 0};
    boundary.set_boundary_rule(env::BoundaryRule::OUTFLOW);
    boundary.set_boundary_rule(env::BoundaryRule::PERIODIC, env::BoundaryNormal::LEFT);
    boundary.set_boundary_rule(env::BoundaryRule::PERIODIC, env::BoundaryNormal::RIGHT);
    boundary.set_boundary_rule(env::BoundaryRule::VELOCITY_REFLECTION, env::BoundaryNormal::TOP);
    boundary.set_boundary_rule(env::BoundaryRule::VELOCITY_REFLECTION, env::BoundaryNormal::BOTTOM);
    // boundary.set_boundary_force(env::Boundary::LennardJonesForce(1,1));

    env.set_boundary(boundary);
    env.set_force(env::LennardJones(1, 1, 2.5), 0);
    env.set_force(env::LennardJones(1, 0.9412, 2.5), 1);
    // env.set_grid_constant(1);
    env.set_gravity_constant(-12.44);
    env.build();

    env::Thermostat thermostat(40, 25, 0.0005);
    thermostat.set_initial_temperature(env);

    auto writer = create_writer(args.output_baseName, args.output_format, args.override);
    Integrator::StoermerVerlet simulator(env, std::move(writer), thermostat);
    simulator.simulate(0, args.duration, args.dt, args.write_freq,1000);

    return 0;
}

