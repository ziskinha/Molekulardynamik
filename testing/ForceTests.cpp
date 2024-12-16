#include <gtest/gtest.h>

#include "../src/env/Environment.h"
#include "../src/env/Force.h"
#include "../src/env/Common.h"

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

    EXPECT_NEAR(calculated_force[0], -0.0194343, 0.001);
    EXPECT_NEAR(calculated_force[1], 0.0291515, 0.001);
    EXPECT_NEAR(calculated_force[2], 0.0291515, 0.001);
}

// Code used for calculating the values
//auto grid2 = md::env::ParticleGrid();
//auto particle3 = md::env::Particle(0, grid2, {1, 5, 4}, {3, 3, 3}, 5, 0);
//auto particle4 = md::env::Particle(0, grid2, {3, 2, 1}, {0, 0, 0}, 5, 0);
//md::env::ForceManager force_manager2 = md::env::ForceManager();
//force_manager2.add_force(md::env::LennardJones(1.5, 2.0, 5), 0);
//force_manager2.init();
//md::vec3 diff = particle4.position - particle3.position;
//md::vec3 calculated_force = force_manager2.evaluate(diff, particle3, particle4);
//
//std::cout << calculated_force[0] << " " << calculated_force[1] << " " << calculated_force[2] << std::endl;