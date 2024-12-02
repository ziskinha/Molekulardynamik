# pragma once

#include "io/IOStrategy.h"
#include "utils/Parse.h"
#include "core/StoermerVerlet.h"
#include "env/Environment.h"
#include "env/Force.h"

using namespace md;


// scenarios used during testing
// not particularly important implementation wise but useful as an example how to interface with our program

inline void four_particle_lennard_jones_test() {
    parse::ProgramArguments args;
    args.output_format = io::OutputFormat::VTK;
    args.benchmark = false;
    args.override = true;

    args.duration = 10;
    args.dt = 0.00001;
    args.write_freq = 1000;


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
    env.set_force(env::LennardJones(5, 0.1));
    env.set_grid_constant(0.1);

    env.build();

    Integrator::StoermerVerlet simulator(env, create_writer(args.output_baseName, args.output_format, args.override));
    simulator.simulate(0, args.duration, args.dt, args.write_freq);
}


inline void four_particle_inverse_force_test() {
    parse::ProgramArguments args;
    args.output_format = io::OutputFormat::VTK;
    args.benchmark = false;
    args.override = true;

    args.duration = 10;
    args.dt = 0.00001;
    args.write_freq = 1000;


    env::Environment env;
    env.add_particle({2.8,2.9,0}, {0,0,0}, 1, 1);
    env.add_particle({0.1,0.15,0}, {0,0,0}, 1, 1);
    env.add_particle({2.5,0.1,0}, {0,0,0}, 1, 1);
    env.add_particle({0.2,2.6,0}, {0,0,0}, 1, 1);

    env::Boundary boundary;
    boundary.extent = {3, 3, 1};
    boundary.origin = {0, 0, 0};
    boundary.set_boundary_rule(env::BoundaryRule::REPULSIVE_FORCE);
    boundary.set_boundary_rule(env::BoundaryRule::OUTFLOW, env::BoundaryNormal::FRONT);
    boundary.set_boundary_rule(env::BoundaryRule::OUTFLOW, env::BoundaryNormal::BACK);
    boundary.set_boundary_force(env::Boundary::InverseDistanceForce(0.5, 0.1, 2));

    env.set_boundary(boundary);
    env.set_force(env::LennardJones(5, 0.1));
    env.set_grid_constant(0.5);

    env.build();

    Integrator::StoermerVerlet simulator(env, create_writer(args.output_baseName, args.output_format, args.override));
    simulator.simulate(0, args.duration, args.dt, args.write_freq);
}

inline void four_particle_periodic_conditions_test() {
    parse::ProgramArguments args;
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
    env.add_particle({1,2,0}, {2,4,0}, 1, 1);
    env.add_particle({2,1,0}, {1,-4,0}, 1, 2);
    env.add_particle({1,1,0}, {-1,4,0}, 1, 3);
    env.add_particle({2,2,0}, {-3,-2,0}, 1, 4);
    env.set_force(env::LennardJones(5, 0.1));
    env.set_boundary(boundary);
    env.set_grid_constant(0.5);

    env.build();

    Integrator::StoermerVerlet simulator(env, create_writer(args.output_baseName, args.output_format, args.override));
    simulator.simulate(0, args.duration, args.dt, args.write_freq);
}

inline void four_particle_reflective_velocity_test() {
    parse::ProgramArguments args;
    args.output_format = io::OutputFormat::VTK;
    args.benchmark = false;
    args.override = true;

    args.duration = 10;
    args.dt = 0.0001;
    args.write_freq = 1000;


    env::Environment env;
    env.add_particle({2.8,2.9,0}, {4,2,0}, 1, 1);
    env.add_particle({0.1,0.15,0}, {-6,1,0}, 1, 1);
    env.add_particle({2.5,0.1,0}, {-1,1,0}, 1, 1);
    env.add_particle({0.2,2.6,0}, {6,2,0}, 1, 1);

    env::Boundary boundary;
    boundary.extent = {3, 3, 1};
    boundary.origin = {0, 0, 0};
    boundary.set_boundary_rule(env::BoundaryRule::VELOCITY_REFLECTION);
    boundary.set_boundary_rule(env::BoundaryRule::OUTFLOW, env::BoundaryNormal::FRONT);
    boundary.set_boundary_rule(env::BoundaryRule::OUTFLOW, env::BoundaryNormal::BACK);

    env.set_boundary(boundary);
    env.set_force(env::LennardJones(5, 0.1));
    env.set_grid_constant(0.5);

    env.build();

    Integrator::StoermerVerlet simulator(env, create_writer(args.output_baseName, args.output_format, args.override));
    simulator.simulate(0, args.duration, args.dt, args.write_freq);
}
