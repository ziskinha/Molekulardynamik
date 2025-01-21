#include <gtest/gtest.h>

#include "../src/env/Environment.h"
#include "../src/env/Force.h"
#include "../src/env/Common.h"
#include "../src/effects/ConstantForce.h"
#include "../src/core/StoermerVerlet.h"

auto grid = md::env::ParticleGrid();
auto particle1 = md::env::Particle(0, grid, {1, 5, 4}, {3, 3, 3}, 5, 0);
auto particle2 = md::env::Particle(0, grid, {3, 2, 1}, {0, 0, 0}, 5, 0);
md::env::ForceManager force_manager1 = md::env::ForceManager();


md::vec3 perform_calculation(md::env::ForceManager force_manager, md::env::Particle p1, md::env::Particle p2) {
    force_manager.init();
    md::vec3 diff = p2.position - p1.position;
    md::vec3 calculated_force = force_manager.evaluate(diff, p1, p2);

    return calculated_force;
}

// check the correctness of InverseSquare calculation
TEST(ForceTest, inverse_square_test) {
    force_manager1.add_force(md::env::InverseSquare(), 0);
    md::vec3 calculated_force = perform_calculation(force_manager1, particle1, particle2);

    EXPECT_NEAR(calculated_force[0], -0.48454708262638735, 1e-5);
    EXPECT_NEAR(calculated_force[1], 0.72682062393958102, 1e-5);
    EXPECT_NEAR(calculated_force[2], 0.72682062393958102, 1e-5);
}

// check the correctness of LennardJones calculation
TEST(ForceTest, Lennard_Jones_test) {
    force_manager1.add_force(md::env::LennardJones(1.0, 1.0, 5), 0);
    md::vec3 calculated_force = perform_calculation(force_manager1, particle1, particle2);

    EXPECT_NEAR(calculated_force[0], 2 * -0.00010243277487532, 1e-5);
    EXPECT_NEAR(calculated_force[1], 3 * 0.00010243277487532, 1e-5);
    EXPECT_NEAR(calculated_force[2], 3 * 0.00010243277487532, 1e-5);
}

// check the correctness of LennardJones calculation when dist_squared > cutoff_radius * cutoff_radius
TEST(ForceTest, lennard_jones_cutoff_radius_test) {
    force_manager1.add_force(md::env::LennardJones(), 0);
    md::vec3 calculated_force = perform_calculation(force_manager1, particle1, particle2);

    EXPECT_TRUE(calculated_force[0] == 0);
    EXPECT_TRUE(calculated_force[1] == 0);
    EXPECT_TRUE(calculated_force[2] == 0);
}

// chek the correctness of mixed forces
TEST(ForceTest, mixed_force_test) {
    auto grid2 = md::env::ParticleGrid();
    auto particle3 = md::env::Particle(0, grid2, {1, 5, 4}, {3, 3, 3}, 5, 0);
    auto particle4 = md::env::Particle(0, grid2, {3, 2, 1}, {0, 0, 0}, 5, 1);
    md::env::ForceManager force_manager2 = md::env::ForceManager();
    force_manager2.add_force(md::env::LennardJones(1.0, 1.0, 5), 0);
    force_manager2.add_force(md::env::LennardJones(2.0, 3.0, 5), 1);
    md::vec3 calculated_force = perform_calculation(force_manager2, particle3, particle4);

    EXPECT_NEAR(calculated_force[0], -0.01832286, 0.001);
    EXPECT_NEAR(calculated_force[1], 0.02748428, 0.001);
    EXPECT_NEAR(calculated_force[2], 0.02748428, 0.001);
}

// test the correctness of gravity application
TEST(ForceTest, gravity_test) {
    md::env::Environment env;
    env.add_cuboid({0.2,0.2,0.2}, {}, {10,10,10}, 0.1, 2);
    env.set_force(md::env::LennardJones(0, 0, 0), 0);

    md::env::Boundary boundary;
    boundary.extent = {10, 10, 10};
    boundary.origin = {0, 0, 0};
    boundary.set_boundary_rule(md::env::BoundaryRule::VELOCITY_REFLECTION);
    env.set_boundary(boundary);
    env.set_grid_constant(1);
    env.build();

    md::env::ConstantForce gravity = md::env::Gravity(-10, {0, 0, 1});

    md::Integrator::StoermerVerlet simulator(env, nullptr, nullptr, md::env::Thermostat(), {gravity});
    simulator.simulate(0, 0.001, 0.001, 0, 10);

    for (auto & p : env.particles()) {
        EXPECT_EQ(p.force[0], 0);
        EXPECT_EQ(p.force[1], 0);
        EXPECT_EQ(p.force[2], -5);
    }
}

// test if pulling force is correctly applied to marked particles
TEST(ForceTest, pulling_force_test1) {
    md::env::Environment env;
    env.add_cuboid({0.2,0.2,0.2}, {}, {10,10,3}, 0.1, 2);
    env.set_force(md::env::LennardJones(0, 0, 0), 0);

    md::env::Boundary boundary;
    boundary.extent = {20, 20, 20};
    boundary.origin = {0, 0, 0};
    boundary.set_boundary_rule(md::env::BoundaryRule::VELOCITY_REFLECTION);
    env.set_boundary(boundary);
    env.set_grid_constant(20);
    env.build();

    md::env::ParticleMarker marker = [] (const md::env::Particle&p) {
        return p.id > 150 && p.id < 160;
    };
    md::env::ConstantForce pulling_force ({1,1, 1}, 2, marker, 0, 1);

    md::Integrator::StoermerVerlet simulator(env, nullptr, nullptr, md::env::Thermostat(), {pulling_force});
    simulator.simulate(0, 0.1, 0.1, 0, -1); // single iteration
    for (auto & p : env.particles()) {
        if (marker(p)) {
            EXPECT_EQ(p.force[0], 2);
            EXPECT_EQ(p.force[1], 2);
            EXPECT_EQ(p.force[2], 2);
        } else {
            EXPECT_EQ(p.force[0], 0);
            EXPECT_EQ(p.force[1], 0);
            EXPECT_EQ(p.force[2], 0);
        }
    }
}

// test if pulling force stops after given duration
TEST(ForceTest, pulling_force_test2) {
    md::env::Environment env;
    env.add_cuboid({0.2,0.2,0.2}, {}, {10,10,3}, 0.1, 2);
    env.set_force(md::env::LennardJones(0, 0, 0), 0);

    md::env::Boundary boundary;
    boundary.extent = {20, 20, 20};
    boundary.origin = {0, 0, 0};
    boundary.set_boundary_rule(md::env::BoundaryRule::VELOCITY_REFLECTION);
    env.set_boundary(boundary);
    env.set_grid_constant(20);
    env.build();

    md::env::ParticleMarker marker = [] (const md::env::Particle&p) {
        return p.id > 150 && p.id < 160;
    };
    md::env::ConstantForce pulling_force ({1, 1, 1}, 2, marker, 0, 1);

    md::Integrator::StoermerVerlet simulator(env, nullptr, nullptr, md::env::Thermostat(), {pulling_force});
    simulator.simulate(0, 1.2, 0.1, 0, -1);
    for (auto & p : env.particles()) {
        EXPECT_EQ(p.force[0], 0);
        EXPECT_EQ(p.force[1], 0);
        EXPECT_EQ(p.force[2], 0);
    }
}