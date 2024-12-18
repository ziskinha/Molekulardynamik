# pragma once

#include "io/IOStrategy.h"
#include "core/StoermerVerlet.h"
#include "env/Environment.h"
#include "env/Force.h"

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

inline void four_particle_periodic_conditions_test() {
    io::ProgramArguments args;
    args.output_format = io::OutputFormat::VTK;
    args.benchmark = false;
    args.override = true;

    args.duration = 10;
    args.dt = 0.00001;
    args.write_freq = 1000;

    env::Boundary boundary;
    boundary.extent = {3, 3, 1};
    boundary.origin = {0, 0, 0};
    boundary.set_boundary_rule(env::BoundaryRule::REPULSIVE_FORCE, env::BoundaryNormal::LEFT);
    boundary.set_boundary_rule(env::BoundaryRule::REPULSIVE_FORCE, env::BoundaryNormal::RIGHT);
    boundary.set_boundary_rule(env::BoundaryRule::PERIODIC, env::BoundaryNormal::TOP);
    boundary.set_boundary_rule(env::BoundaryRule::PERIODIC, env::BoundaryNormal::BOTTOM);
    boundary.set_boundary_rule(env::BoundaryRule::OUTFLOW, env::BoundaryNormal::FRONT);
    boundary.set_boundary_rule(env::BoundaryRule::OUTFLOW, env::BoundaryNormal::BACK);
    boundary.set_boundary_force(env::Boundary::InverseDistanceForce(0.5, 0.1, 2));

    env::Environment env;
    env.add_particle({1, 2, 0}, {2, 4, 0}, 1, 1);
    env.add_particle({2, 1, 0}, {1, -4, 0}, 1, 2);
    env.add_particle({1, 1, 0}, {-1, 4, 0}, 1, 3);
    env.add_particle({2, 2, 0}, {-3, -2, 0}, 1, 4);
    env.set_force(env::LennardJones(5, 0.1, 0.5), 1);
    env.set_boundary(boundary);
    env.set_grid_constant(0.5);

    env.build();

    Integrator::StoermerVerlet simulator(env, create_writer(args.output_baseName, args.output_format, args.override));
    simulator.simulate(0, args.duration, args.dt, args.write_freq);
}

inline void four_particle_reflective_velocity_test() {
    io::ProgramArguments args;
    args.output_format = io::OutputFormat::VTK;
    args.benchmark = false;
    args.override = true;

    args.duration = 10;
    args.dt = 0.0001;
    args.write_freq = 1000;

    env::Environment env;
    env.add_particle({2.8, 2.9, 0}, {4, 2, 0}, 1, 1);
    env.add_particle({0.1, 0.15, 0}, {-6, 1, 0}, 1, 1);
    // env.add_particle({2.5, 0.1, 0}, {-1, 1, 0}, 1, 1);
    // env.add_particle({0.2, 2.6, 0}, {6, 2, 0}, 1, 1);

    env::Boundary boundary;
    boundary.extent = {3, 3, 1};
    boundary.origin = {0, 0, 0};
    boundary.set_boundary_rule(env::BoundaryRule::VELOCITY_REFLECTION);
    boundary.set_boundary_rule(env::BoundaryRule::OUTFLOW, env::BoundaryNormal::FRONT);
    boundary.set_boundary_rule(env::BoundaryRule::OUTFLOW, env::BoundaryNormal::BACK);

    env.set_boundary(boundary);
    env.set_force(env::LennardJones(5, 0.1, 0.5), 1);
    env.set_grid_constant(3);

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

// Task 2 on the worksheet 4
inline void ws4_task2_small() {
    io::ProgramArguments args;
    args.output_format = io::OutputFormat::VTK;
    args.benchmark = false;
    args.override = true;
    args.output_baseName = "output";

    args.duration = 25;
    args.dt = 0.0005;
    args.write_freq = 100;

    env::Environment env;
    env::Boundary boundary;
    // boundary size
    boundary.extent = {63, 36, 1};
    boundary.origin = {0, 0, 0};

    //boundary conditions
    boundary.set_boundary_rule(env::BoundaryRule::OUTFLOW);
    boundary.set_boundary_rule(env::BoundaryRule::PERIODIC, env::BoundaryNormal::LEFT);
    boundary.set_boundary_rule(env::BoundaryRule::PERIODIC, env::BoundaryNormal::RIGHT);
    boundary.set_boundary_rule(env::BoundaryRule::REPULSIVE_FORCE, env::BoundaryNormal::TOP);
    boundary.set_boundary_rule(env::BoundaryRule::REPULSIVE_FORCE, env::BoundaryNormal::BOTTOM);

    boundary.set_boundary_force(env::Boundary::LennardJonesForce(1, 1));

    env.set_boundary(boundary);

    // Liquid 1, type = 0
    env.add_cuboid({0.6, 2, 0}, {0, 0, 0}, {50, 14, 1}, 0, 1.2, 1, 2, 0);
    env.set_force(env::LennardJones(1, 1, 2.5), 0);
    // Liquid 2, type = 1
    env.add_cuboid({0.6, 19, 0}, {0, 0, 0}, {50, 14, 1}, 0, 1.2, 2, 2, 1);
    env.set_force(env::LennardJones(1, 0.9412, 2.5), 1);

    env.set_grid_constant(2.5);
    env.set_gravity_constant(-12.44);
    env.build();

    env::Thermostat thermostat(40);
    thermostat.set_initial_temperature(env);

    auto writer = create_writer(args.output_baseName, args.output_format, args.override);
    Integrator::StoermerVerlet simulator(env, std::move(writer), nullptr, thermostat);
    // simulator.benchmark(0, args.duration, args.dt, 1000);
    simulator.simulate(0, args.duration, args.dt, args.write_freq, 1000);

}

inline void ws4_task2_big() {
    io::ProgramArguments args;
    args.output_format = io::OutputFormat::VTK;
    args.benchmark = false;
    args.override = true;
    args.output_baseName = "output";

    args.duration = 5;
    args.dt = 0.0005;
    args.write_freq = 100;

    env::Environment env;

    env::Boundary boundary;
    // boundary size
    boundary.extent = {300, 54, 1};
    boundary.origin = {0, 0, 0};

    //boundary conditions
    boundary.set_boundary_rule(env::BoundaryRule::OUTFLOW);
    boundary.set_boundary_rule(env::BoundaryRule::PERIODIC, env::BoundaryNormal::LEFT);
    boundary.set_boundary_rule(env::BoundaryRule::PERIODIC, env::BoundaryNormal::RIGHT);
    boundary.set_boundary_rule(env::BoundaryRule::VELOCITY_REFLECTION, env::BoundaryNormal::TOP);
    boundary.set_boundary_rule(env::BoundaryRule::VELOCITY_REFLECTION, env::BoundaryNormal::BOTTOM);

    boundary.set_boundary_force(env::Boundary::LennardJonesForce(1, 1.2));

    env.set_boundary(boundary);

    // Liquid 1, type = 0
    env.add_cuboid({0.6, 2, 0}, {0, 0, 0}, {250, 20, 1}, 0, 1.2, 1, 2, 0);
    env.set_force(env::LennardJones(1, 1.2, 3), 0);
    // Liquid 2, type = 1
    env.add_cuboid({0.6, 27, 0}, {0, 0, 0}, {250, 20, 1}, 0, 1.2, 2, 2, 1);
    env.set_force(env::LennardJones(1, 1.1, 3), 1);

    env.set_grid_constant(3);
    env.set_gravity_constant(-12.44);
    env.build();

    env::Thermostat thermostat(40);
    thermostat.set_initial_temperature(env);

    auto writer = create_writer(args.output_baseName, args.output_format, args.override);
    Integrator::StoermerVerlet simulator(env, std::move(writer), nullptr, thermostat);
    simulator.simulate(0, args.duration, args.dt, args.write_freq, 1000);
    // simulator.benchmark(0, args.duration, args.dt, 1000);

}

// Total execution time:45531.1 ms
// Average execution time per step: 4.5522 ms
// Number of particles: 10000
// Particle modifications: 100010000
// MUPS/s
// 2222444
// Average execution time per step: 4.38718 ms
// Number of particles: 10000
// Particle modifications: 100010000
// MUPS/s
// 2325813
// Total execution time:43904.7 ms
// Average execution time per step: 4.38959 ms
// Number of particles: 10000
// Particle modifications: 100010000
// MUPS/s
// 2325813

