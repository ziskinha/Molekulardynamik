#include <gtest/gtest.h>

#include "../src/core/StoermerVerlet.h"
#include "../src/env/Boundary.h"
#include "../src/env/Environment.h"
#include "../src/env/Particle.h"

// Sets up the environment for each test
void setUp(md::env::Environment &env, md::env::Boundary boundary, bool use_standard_particles) {
    boundary.origin = {0, 0, 0};
    boundary.extent = {10, 10, 1};
    boundary.set_boundary_force(md::env::Boundary::LennardJonesForce(1, 1));
    env.set_force(md::env::LennardJones(1, 1, 2.5), 0);
    env.set_grid_constant(2.5);

    if (use_standard_particles) {
        env.add_particle({2, 4, 0}, {-4, 4, 0}, 1, 0);
        env.add_particle({4, 8, 0}, {4, 4, 0}, 1, 0);
        env.add_particle({8, 6, 0}, {4, -4, 0}, 1, 0);
        env.add_particle({6, 2, 0}, {-4, -4, 0}, 1, 0);
    }

    env.set_boundary(boundary);
    env.build();
};

// tests if the outflow condition works correctly
TEST(BoundaryConditionsTest, outflow_condition_test) {
    md::env::Environment env;
    md::env::Boundary boundary;
    boundary.set_boundary_rule(md::env::BoundaryRule::OUTFLOW);
    setUp(env, boundary, true);

    md::Integrator::StoermerVerlet simulator(env);
    simulator.simulate(0, 1, 0.001);

    for (int i = 0; i < 4; i++) {
        EXPECT_TRUE(env.operator[](i).state == md::env::Particle::DEAD);
    }
}

// tests if the periodic condition works correctly
TEST(BoundaryConditionsTest, periodic_condition_test) {
    md::env::Environment env;
    md::env::Boundary boundary;
    boundary.set_boundary_rule(md::env::BoundaryRule::PERIODIC);

    // they should apply force on each other
    env.add_particle({5, 1, 0}, {0, -1, 0}, 1, 0);
    env.add_particle({5, 9, 0}, {0, 1, 0}, 1, 0);
    // this one should just be teleported
    env.add_particle({1, 5, 0}, {-1, 0, 0}, 1, 0);

    setUp(env, boundary, false);

    md::Integrator::StoermerVerlet simulator(env);
    simulator.simulate(0, 2, 0.001);

    EXPECT_NEAR(env.operator[](0).position[1], 2, 0.1);
    EXPECT_NEAR(env.operator[](1).position[1], 8, 0.1);
    EXPECT_NEAR(env.operator[](2).position[0], 9, 0.1);
}

// tests if the repulsive force condition works correctly
TEST(BoundaryConditionsTest, repulsive_force_condition_test) {
    md::env::Environment env;
    md::env::Boundary boundary;
    boundary.set_boundary_rule(md::env::BoundaryRule::OUTFLOW);
    boundary.set_boundary_rule(md::env::BoundaryRule::REPULSIVE_FORCE, md::env::BoundaryNormal::BOTTOM);
    env.add_particle({5, 2.1225, 0}, {0, -1, 0}, 1, 0);
    setUp(env, boundary, false);

    md::Integrator::StoermerVerlet simulator(env);
    simulator.simulate(0, 0.999, 0.0001);

    // distance where repulsive force should not be applied
    EXPECT_TRUE(env.operator[](0).force[0] == 0);
    EXPECT_TRUE(env.operator[](0).force[1] == 0);
    EXPECT_TRUE(env.operator[](0).force[2] == 0);

    simulator.simulate(0, 0.0002, 0.0001);
    // repulsive force should be applied
    EXPECT_TRUE(env.operator[](0).force[0] == 0);
    EXPECT_NEAR(env.operator[](0).force[1], 0.0035479, 0.01);
    EXPECT_TRUE(env.operator[](0).force[2] == 0);
}

// tests if the velocity reflection condition works correctly
TEST(BoundaryConditionsTest, velocity_reflection_condition_test) {
    md::env::Environment env;
    md::env::Boundary boundary;
    boundary.set_boundary_rule(md::env::BoundaryRule::VELOCITY_REFLECTION);
    setUp(env, boundary, true);

    md::Integrator::StoermerVerlet simulator(env);
    simulator.simulate(0, 1, 0.001, 1000);

    EXPECT_NEAR(env.operator[](0).position[0], 2, 0.01);
    EXPECT_NEAR(env.operator[](0).position[1], 8, 0.01);

    EXPECT_NEAR(env.operator[](1).position[0], 8, 0.01);
    EXPECT_NEAR(env.operator[](1).position[1], 8, 0.01);

    EXPECT_NEAR(env.operator[](2).position[0], 8, 0.01);
    EXPECT_NEAR(env.operator[](2).position[1], 2, 0.01);

    EXPECT_NEAR(env.operator[](3).position[0], 2, 0.01);
    EXPECT_NEAR(env.operator[](3).position[1], 2, 0.01);
}

// tests if correct condition was selected when particle passes a boundary at the corner
TEST(BoundaryConditionsTest, corner_condition_test) {
    md::env::Environment env;
    md::env::Boundary boundary;
    boundary.set_boundary_rule(md::env::BoundaryRule::OUTFLOW);
    boundary.set_boundary_rule(md::env::BoundaryRule::PERIODIC, md::env::BoundaryNormal::TOP);
    boundary.origin = {0, 0, 0};
    boundary.extent = {3, 3, 1};
    boundary.set_boundary_force(md::env::Boundary::LennardJonesForce(5, 0.1));
    env.set_grid_constant(1);
    env.set_boundary(boundary);
    env.add_particle({1.1, 2.5, 0}, {-1, 1, 0}, 1, 0);
    env.build();

    md::Integrator::StoermerVerlet simulator(env);
    simulator.simulate(0, 1, 0.001, 1000);

    EXPECT_FALSE(env.operator[](0).state == md::env::Particle::DEAD);
    EXPECT_NEAR(env.operator[](0).position[0], 0.1, 0.01);
    EXPECT_NEAR(env.operator[](0).position[1], 0.5, 0.01);

    simulator.simulate(0, 1, 0.001, 10000);
    EXPECT_TRUE(env.operator[](0).state == md::env::Particle::DEAD);
}