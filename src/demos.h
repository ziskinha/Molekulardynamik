# pragma once

#include "io/IOStrategy.h"
#include "core/StoermerVerlet.h"
#include "env/Environment.h"
#include "env/Force.h"
#include "core/Statistics.h"

using namespace md;


// scenarios used during testing
// not particularly important implementation wise but useful as an example how to interface with our program

inline void four_particle_lennard_jones_test() {
    io::ProgramArguments args;
    args.output_format = io::OutputFormat::VTK;
    args.benchmark = false;
    args.override = true;

    args.duration = 10;
    args.dt = 0.00001;
    args.write_freq = 1000;

    env::Environment env;
    env.add_particle({2.8, 2.9, 0}, {4, 2, 0}, 1, 1);
    env.add_particle({0.1, 0.15, 0}, {-6, 1, 0}, 1, 1);
    env.add_particle({2.5, 0.1, 0}, {-1, 1, 0}, 1, 1);
    env.add_particle({0.2, 2.6, 0}, {6, 2, 0}, 1, 1);

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
    simulator.simulate(0, args.duration, args.dt, args.write_freq);
}


inline void four_particle_inverse_force_test() {
    io::ProgramArguments args;
    args.output_format = io::OutputFormat::VTK;
    args.benchmark = false;
    args.override = true;

    args.duration = 10;
    args.dt = 0.00001;
    args.write_freq = 1000;

    env::Environment env;
    env.add_particle({2.8, 2.9, 0}, {0, 0, 0}, 1, 1);
    env.add_particle({0.1, 0.15, 0}, {0, 0, 0}, 1, 1);
    env.add_particle({2.5, 0.1, 0}, {0, 0, 0}, 1, 1);
    env.add_particle({0.2, 2.6, 0}, {0, 0, 0}, 1, 1);

    env::Boundary boundary;
    boundary.extent = {3, 3, 1};
    boundary.origin = {0, 0, 0};
    boundary.set_boundary_rule(env::BoundaryRule::REPULSIVE_FORCE);
    boundary.set_boundary_rule(env::BoundaryRule::OUTFLOW, env::BoundaryNormal::FRONT);
    boundary.set_boundary_rule(env::BoundaryRule::OUTFLOW, env::BoundaryNormal::BACK);
    boundary.set_boundary_force(env::Boundary::InverseDistanceForce(0.5, 0.1, 2));

    env.set_boundary(boundary);
    env.set_force(env::LennardJones(5, 0.1, 0.5), 1);
    env.set_grid_constant(0.5);

    env.build();

    Integrator::StoermerVerlet simulator(env, create_writer(args.output_baseName, args.output_format, args.override));
    simulator.simulate(0, args.duration, args.dt, args.write_freq);
}

inline void periodic_force_mixing_test() {
    io::ProgramArguments args;
    args.output_format = io::OutputFormat::VTK;
    args.benchmark = false;
    args.override = true;
    args.output_baseName = "output";

    args.duration = 20;
    args.dt = 0.0001;
    args.write_freq = 200;

    env::Environment env;
    env.add_particle({0.3, 0.3, 0}, {0, 0, 0}, 1, 1);
    env.add_particle({2.7, 2.7, 0}, {0, 0, 0}, 200, 4);
    env.add_particle({0.3, 2.7, 0}, {0, 0, 0}, 1, 2);
    env.add_particle({2.7, 0.3, 0}, {0, 0, 0}, 1, 3);

    env::Boundary boundary;
    boundary.extent = {3, 3, 1};
    boundary.origin = {0, 0, 0};
    boundary.set_boundary_rule(env::BoundaryRule::OUTFLOW);
    boundary.set_boundary_rule(env::BoundaryRule::PERIODIC, env::BoundaryNormal::LEFT);
    boundary.set_boundary_rule(env::BoundaryRule::PERIODIC, env::BoundaryNormal::RIGHT);
    boundary.set_boundary_rule(env::BoundaryRule::PERIODIC, env::BoundaryNormal::TOP);
    boundary.set_boundary_rule(env::BoundaryRule::PERIODIC, env::BoundaryNormal::BOTTOM);

    env.set_boundary(boundary);
    env.set_force(env::LennardJones(5, 0.2, 1), 1);
    env.set_force(env::LennardJones(5, 0.2, 1), 2);
    env.set_force(env::LennardJones(5, 0.2, 1), 3);
    env.set_force(env::LennardJones(1, 0.5, 1), 4);

    env.set_grid_constant(1);

    env.build();

    auto writer = create_writer(args.output_baseName, args.output_format, args.override);
    Integrator::StoermerVerlet simulator(env, std::move(writer));
    simulator.simulate(0, args.duration, args.dt, args.write_freq);
}


inline void thermostat_test() {
    io::ProgramArguments args;
    args.output_format = io::OutputFormat::VTK;
    args.benchmark = false;
    args.override = true;
    args.output_baseName = "output";

    args.duration = 20;
    args.dt = 0.0001;
    args.write_freq = 200;

    env::Environment env;
    env.add_cuboid({2, 2, 0}, {0, 0, 0}, {30, 30, 1}, 0, 1.1225, 1, 2);

    env::Boundary boundary;
    boundary.extent = {38, 38, 1};
    boundary.origin = {0, 0, 0};
    boundary.set_boundary_rule(env::BoundaryRule::OUTFLOW);
    boundary.set_boundary_rule(env::BoundaryRule::PERIODIC, env::BoundaryNormal::LEFT);
    boundary.set_boundary_rule(env::BoundaryRule::PERIODIC, env::BoundaryNormal::RIGHT);
    boundary.set_boundary_rule(env::BoundaryRule::PERIODIC, env::BoundaryNormal::TOP);
    boundary.set_boundary_rule(env::BoundaryRule::PERIODIC, env::BoundaryNormal::BOTTOM);

    env.set_boundary(boundary);
    env.set_force(env::LennardJones(5, 1, 1), 0);
    env.set_grid_constant(1);
    env.build();

    env::Thermostat thermostat(40, -1, -1);
    thermostat.set_initial_temperature(env);

    auto writer = create_writer(args.output_baseName, args.output_format, args.override);
    Integrator::StoermerVerlet simulator(env, std::move(writer), nullptr, thermostat);
    simulator.simulate(0, args.duration, args.dt, args.write_freq, 1000);
}


inline void nano_scale_simulation() {
    io::ProgramArguments args;
    args.output_format = io::OutputFormat::VTK;
    args.benchmark = false;
    args.override = true;
    args.output_baseName = "output";
    args.duration = 20;
    args.dt = 0.0005;
    args.write_freq = 10;

    env::Boundary boundary;
    boundary.extent = {30, 30, 12};
    boundary.set_boundary_rule(env::BoundaryRule::PERIODIC, env::BoundaryNormal::TOP);
    boundary.set_boundary_rule(env::BoundaryRule::PERIODIC, env::BoundaryNormal::BOTTOM);
    boundary.set_boundary_rule(env::BoundaryRule::PERIODIC, env::BoundaryNormal::FRONT);
    boundary.set_boundary_rule(env::BoundaryRule::PERIODIC, env::BoundaryNormal::BACK);

    env::Environment env;
    env.set_boundary(boundary);

    // walls
    env.add_cuboid({1, 0.5, 0.5}, {}, {2, 30, 12}, 1, 1, 0, 0, env::Dimension::THREE, env::Particle::STATIONARY);
    env.add_cuboid({27.2, 0.5, 0.5}, {}, {2, 30, 12}, 1, 1, 0, 0, env::Dimension::THREE, env::Particle::STATIONARY);
    env.set_force(env::LennardJones(2, 1.1, 2.5), 0);

    // fluid
    env.add_cuboid({3.2, 0.6, 0.6}, {}, {20, 25, 10}, 1.2, 1, 0, 1);
    env.set_force(env::LennardJones(1, 1, 2.5), 1);
    env.build();

    env::ConstantForce gravity = env::Gravity(-0.8, {0, 1, 0});

    env::Thermostat thermostat(40);
    thermostat.set_initial_temperature(env);

    auto stats = std::make_unique<core::NanoFlowStatistics>(10000, 50);
    auto writer = create_writer(args.output_baseName, args.output_format, args.override);

    Integrator::StoermerVerlet simulator(env, std::move(writer), nullptr, thermostat, {gravity}, std::move(stats));
    simulator.simulate(0, args.duration, args.dt, args.write_freq, 10);
}


inline void membrane_simulation() {
    io::ProgramArguments args;
    args.output_format = io::OutputFormat::VTK;
    args.benchmark = false;
    args.override = true;
    args.output_baseName = "output";
    args.duration = 500;
    args.dt = 0.01;
    args.write_freq = 40;

    env::Boundary boundary;
    boundary.extent = {148, 148, 148};
    boundary.set_boundary_rule(env::BoundaryRule::VELOCITY_REFLECTION);

    env::Environment env;
    env.set_boundary(boundary);
    env.add_membrane({15, 15, 1.5}, {0, 0, 0}, {50, 50, 1}, 2.2, 1, 300, 4 * 1.1225);
    env.set_force(env::LennardJones(1, 1, 1.1225), 0);
    env.build();

    env::ConstantForce gravity = env::Gravity(-0.001);
    env::ConstantForce pull_force ({0, 0, 1}, 0.8, env::MarkBox({15 + 17 * 2.2, 15 + 24 * 2.2, 0}, {15 + 18.1 * 2.2, 15 + 25.1 * 2.2, 2}), 0, 150);

    auto writer = create_writer(args.output_baseName, args.output_format, args.override);
    Integrator::StoermerVerlet simulator(env, std::move(writer), nullptr, env::Thermostat(), {gravity, pull_force});
    simulator.simulate(0, args.duration, args.dt, args.write_freq);
}
