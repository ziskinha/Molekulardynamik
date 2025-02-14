#include <gtest/gtest.h>

#include "core/StoermerVerlet/StoermerVerlet.h"
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

// tests if the outflow condition works correctly with a single simulation iteration
TEST(BoundaryConditionsTest, outflow_condition_test_simple) {
    md::env::Environment env;
    md::env::Boundary boundary;
    boundary.set_boundary_rule(md::env::BoundaryRule::OUTFLOW);
    env.add_particle({0, 0, 0}, {-1, 0, 0}, 1, 0);
    setUp(env, boundary, false);

    ASSERT_TRUE(env.operator[](0).state == md::env::Particle::ALIVE);

    md::Integrator::StoermerVerlet simulator(env);
    simulator.simulate(0, 1, 0.001);

    EXPECT_TRUE(env.operator[](0).state == md::env::Particle::DEAD);
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

// tests if the periodic condition works correctly with a single simulation iteration
TEST(BoundaryConditionsTest, simple_periodic_test) {
    md::env::Environment env;
    md::env::Boundary boundary;
    boundary.set_boundary_rule(md::env::BoundaryRule::PERIODIC);

    env.add_particle({0, 1, 0}, {-1, 0, 0}, 1, 0); // gets teleported
    env.add_particle({1, 6, 0}, {0, 0, 0}, 1, 0); // attracts particle on other side
    env.add_particle({9, 6, 0}, {0, 0, 0}, 1, 0);

    setUp(env, boundary, false);

    md::Integrator::StoermerVerlet simulator(env);
    simulator.simulate(0, 0.001, 0.001); // single iteration

    EXPECT_NEAR(env[0].position[0], 9.999, 0.000001);
    EXPECT_NEAR(env[1].force[0], -0.332783025, 0.000001);
    EXPECT_NEAR(env[2].force[0], 0.332783025, 0.000001);
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

// tests if the repulsive force condition works correctly with a single simulation iteration
TEST(BoundaryConditionsTest, simple_repulive_force_test) {
    md::env::Environment env;
    md::env::Boundary boundary;
    boundary.set_boundary_rule(md::env::BoundaryRule::OUTFLOW);
    boundary.set_boundary_rule(md::env::BoundaryRule::REPULSIVE_FORCE, md::env::BoundaryNormal::BOTTOM);
    boundary.set_boundary_rule(md::env::BoundaryRule::REPULSIVE_FORCE, md::env::BoundaryNormal::LEFT);

    env.add_particle({0.5, 0.5, 0}, {0, 0, 0}, 1, 0);

    setUp(env, boundary, false);

    md::Integrator::StoermerVerlet simulator(env);
    simulator.simulate(0, 0.001, 0.001); // single iteration

    EXPECT_NEAR(env[0].force[0], 390144, 0.000001);
    EXPECT_NEAR(env[0].force[1], 390144, 0.000001);
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

// test periodic boundary conditon edge case that requires multiple teleports
TEST(BoundaryConditionsTest, periodi_condition_edge_case) {
    md::env::Environment env;
    md::env::Boundary boundary;
    boundary.set_boundary_rule(md::env::BoundaryRule::PERIODIC);
    boundary.origin = {0, 0, 0};
    boundary.extent = {3, 3, 3};
    env.set_grid_constant(1);
    env.set_boundary(boundary);
    env.add_particle({2.995, 2.985, 2.99}, {2, 2, 2}, 1, 0);
    env.build();

    md::Integrator::StoermerVerlet simulator(env);
    simulator.simulate(0, 0.03, 0.01, 1000);

    int i = 0;

    EXPECT_NEAR(env.operator[](0).position[0], 0.015, 1e5);
    EXPECT_NEAR(env.operator[](0).position[0], 0.005, 1e5);
    EXPECT_NEAR(env.operator[](0).position[0], 0.01, 1e5);

}