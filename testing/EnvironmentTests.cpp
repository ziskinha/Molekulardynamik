#include <gtest/gtest.h>

#include "../src/env/Environment.h"
#include "../src/utils/MaxwellBoltzmannDistribution.h"
#include "core/StoermerVerlet/StoermerVerlet.h"
#include "core/Statistics.h"

auto grid = md::env::ParticleGrid();
auto particle1 = md::env::Particle(0, grid, {1, 5, 4}, {3, 3, 3}, 5, 0);
auto particle2 = md::env::Particle(0, grid, {3, 2, 1}, {0, 0, 0}, 5, 0);
auto particle3 = md::env::Particle(0, grid, {10, -1, 4}, {5, 0, 0}, 10, 0);

// check if the size() method works as it should
TEST(EnvironmentTest, size_test) {
    md::env::Environment env;
    md::env::Boundary boundary;
    boundary.extent = {20,20,20};
    boundary.origin = {-10,-10, -10};

    env.add_particle({1, 5, 4}, {3, 3, 3}, 5, 0);
    env.add_particle({3, 2, 1}, {0, 0, 0}, 5, 0);
    env.add_particle({9, -1, 4}, {5, 0, 0}, 10, 0);
    env.add_particle({3, 8, 1}, {0, 0, 0}, 3, 0,  md::env::Particle::STATIONARY);
    env.add_particle({-4, .3, 2}, {0, 0, 0}, 7, 0, md::env::Particle::STATIONARY);
    env.add_particle({8, 1, 2}, {0, -20, 0}, 200, 0, md::env::Particle::DEAD);

    env.set_boundary(boundary);
    env.set_grid_constant(1);
    env.build();

    EXPECT_EQ(env.size(md::env::Particle::ALIVE), 3);
    EXPECT_EQ(env.size(md::env::Particle::DEAD), 1);
    EXPECT_EQ(env.size(md::env::Particle::STATIONARY), 2);
    EXPECT_EQ(env.size(md::env::Particle::ALIVE | md::env::Particle::STATIONARY), 5);
    EXPECT_EQ(env.size(md::env::Particle::ALIVE | md::env::Particle::STATIONARY | md::env::Particle::DEAD), 6);
}

// test whether stationary particles stay stationary
TEST(EnvironmentTest, stationary_particles_test) {
    md::env::Boundary boundary;
    boundary.extent = {6,6,6};
    boundary.origin = {0,0, 0};
    boundary.set_boundary_rule(md::env::BoundaryRule::VELOCITY_REFLECTION);

    md::env::Environment env;
    env.add_particle({2, 2, 2}, {0, 0, 0}, 1, 0,  md::env::Particle::STATIONARY);
    env.add_particle({4,4,4}, {0, 0, 0}, 1, 0, md::env::Particle::STATIONARY);

    env.add_cuboid({1,1,1}, {}, {4,4,4}, 1, 1);
    env.set_force(md::env::InverseSquare(2, 4), 0);
    env.set_boundary(boundary);

    env.build();

    EXPECT_EQ(env[0].position[0], 2);
    EXPECT_EQ(env[0].position[1], 2);
    EXPECT_EQ(env[0].position[2], 2);

    EXPECT_EQ(env[1].position[0], 4);
    EXPECT_EQ(env[1].position[1], 4);
    EXPECT_EQ(env[1].position[2], 4);

    md::Integrator::StoermerVerlet simulator(env);
    simulator.simulate(0, 2, 0.01);

    EXPECT_EQ(env[0].position[0], 2);
    EXPECT_EQ(env[0].position[1], 2);
    EXPECT_EQ(env[0].position[2], 2);

    EXPECT_EQ(env[1].position[0], 4);
    EXPECT_EQ(env[1].position[1], 4);
    EXPECT_EQ(env[1].position[2], 4);
}


// check if container returns correct particle from the container
TEST(EnvironmentTest, index_test) {
    auto part = md::env::ParticleCreateInfo({1, 5, 4}, {3, 3, 3}, 5, 0);
    md::env::Environment env;
    env.add_particles({part});
    env.build();

    EXPECT_TRUE(env[0].position[0] == 1);
    EXPECT_TRUE(env[0].position[1] == 5);
    EXPECT_TRUE(env[0].position[2] == 4);
    EXPECT_TRUE(env[0].velocity[0] == 3);
    EXPECT_TRUE(env[0].velocity[1] == 3);
    EXPECT_TRUE(env[0].velocity[2] == 3);
    EXPECT_TRUE(env[0].mass == 5);
    EXPECT_TRUE(env[0].type == 0);
}

// check if new particle can be added to the container
TEST(EnvironmentTest, paricle_add_test) {
    auto part = md::env::ParticleCreateInfo({7, 7, 7}, {3, 2, 1}, 1, 0);
    md::env::Environment env;
    env.add_particles({part});
    env.build();

    EXPECT_TRUE(env[0].position[0] == 7);
    EXPECT_TRUE(env[0].position[1] == 7);
    EXPECT_TRUE(env[0].position[2] == 7);

    EXPECT_TRUE(env[0].velocity[0] == 3);
    EXPECT_TRUE(env[0].velocity[1] == 2);
    EXPECT_TRUE(env[0].velocity[2] == 1);

    EXPECT_TRUE(env[0].mass == 1);
    EXPECT_TRUE(env[0].type == 0);
}

// test for checking if adding a cuboid succeeds
TEST(EnvironmentTest, add_cuboid_test) {
    auto part = md::env::CuboidCreateInfo({0, 0, 0}, {1, 1, 0}, {2, 1, 1}, 1, 5, 0, 0, md::env::Dimension::TWO);

    md::env::Environment env;
    env.add_cuboid(part);
    env.build();

    ASSERT_TRUE(env.size(md::env::Particle::ALIVE) == 2);

    EXPECT_TRUE(env[0].position[0] == 0);
    EXPECT_TRUE(env[0].position[1] == 0);
    EXPECT_TRUE(env[0].position[2] == 0);

    EXPECT_TRUE(env[0].velocity[0] == 1);
    EXPECT_TRUE(env[0].velocity[1] == 1);
    EXPECT_TRUE(env[0].velocity[2] == 0);

    EXPECT_TRUE(env[0].mass == 5);
    EXPECT_TRUE(env[0].type == 0);

    EXPECT_TRUE(env[1].position[0] == 1);
    EXPECT_TRUE(env[1].position[1] == 0);
    EXPECT_TRUE(env[1].position[2] == 0);

    EXPECT_TRUE(env[1].velocity[0] == 1);
    EXPECT_TRUE(env[1].velocity[1] == 1);
    EXPECT_TRUE(env[1].velocity[2] == 0);

    EXPECT_TRUE(env[1].mass == 5);
    EXPECT_TRUE(env[1].type == 0);
}

TEST(EnvironmentTest,add_membrane_test){
    md::env::Environment env;
    env.add_membrane({0,0,0},{1,1,0},{2,1,1},5,2,1,1,0);
    env.build();
    ASSERT_TRUE(env.size(md::env::Particle::ALIVE) == 2);
    EXPECT_TRUE(env[0].position[0] == 0);
    EXPECT_TRUE(env[0].position[1] == 0);
    EXPECT_TRUE(env[0].position[2] == 0);

    EXPECT_TRUE(env[0].velocity[0] == 1);
    EXPECT_TRUE(env[0].velocity[1] == 1);
    EXPECT_TRUE(env[0].velocity[2] == 0);

    EXPECT_TRUE(env[0].mass == 2);
    EXPECT_TRUE(env[0].type == 0);

    EXPECT_TRUE(env[0].force[0] == 0);
    EXPECT_TRUE(env[0].force[1] == 0);
    EXPECT_TRUE(env[0].force[2] == 0);

    EXPECT_TRUE(env[1].position[0] == 5);
    EXPECT_TRUE(env[1].position[1] == 0);
    EXPECT_TRUE(env[1].position[2] == 0);

    EXPECT_TRUE(env[1].velocity[0] == 1);
    EXPECT_TRUE(env[1].velocity[1] == 1);
    EXPECT_TRUE(env[1].velocity[2] == 0);

    EXPECT_TRUE(env[1].mass == 2);
    EXPECT_TRUE(env[1].type == 0);

    EXPECT_TRUE(env[1].force[0] == 0);
    EXPECT_TRUE(env[1].force[1] == 0);
    EXPECT_TRUE(env[1].force[2] == 0);
}