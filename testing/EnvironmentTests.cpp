#include <gtest/gtest.h>

#include "../src/env/Environment.h"
#include "../src/utils/MaxwellBoltzmannDistribution.h"

auto grid = md::env::ParticleGrid();
auto particle1 = md::env::Particle(0, grid, {1, 5, 4}, {3, 3, 3}, 5, 0);
auto particle2 = md::env::Particle(0, grid, {3, 2, 1}, {0, 0, 0}, 5, 0);
auto particle3 = md::env::Particle(0, grid, {10, -1, 4}, {5, 0, 0}, 10, 0);

// check if the size() method works as it should
TEST(EnvironmentTest, size_test) {
    md::env::Environment env;
    env.add_particle({1, 5, 4}, {3, 3, 3}, 5, 0);
    env.add_particle({3, 2, 1}, {0, 0, 0}, 5, 0);
    env.add_particle({10, -1, 4}, {5, 0, 0}, 10, 0);
    env.build();

    EXPECT_TRUE(env.size() == 3);
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
    auto part = md::env::CuboidCreateInfo({0, 0, 0}, {1, 1, 1}, {2, 1, 1}, 2, 1, 5, md::env::Dimension::TWO);
    md::env::Environment env;
    env.add_cuboid({part});
    resetRandomEngine();
    md::vec3 bltzm = maxwellBoltzmannDistributedVelocity(2, 2);
    md::vec3 cns1 = {bltzm[0] + 1, bltzm[1] + 1, bltzm[2] + 1};
    bltzm = maxwellBoltzmannDistributedVelocity(2, 2);
    md::vec3 cns2 = {bltzm[0] + 1, bltzm[1] + 1, bltzm[2] + 1};
    env.build();

    ASSERT_TRUE(env.size() == 2);
    EXPECT_TRUE(env[0].position[0] == 0);
    EXPECT_TRUE(env[0].position[1] == 0);
    EXPECT_TRUE(env[0].position[2] == 0);

    EXPECT_NEAR(env[0].velocity[0], cns1[0], 1e-5);
    EXPECT_NEAR(env[0].velocity[1], cns1[1], 1e-5);
    EXPECT_NEAR(env[0].velocity[2], cns1[2], 1e-5);

    EXPECT_TRUE(env[0].mass == 5);
    EXPECT_TRUE(env[0].type == 0);

    EXPECT_TRUE(env[1].position[0] == 1);
    EXPECT_TRUE(env[1].position[1] == 0);
    EXPECT_TRUE(env[1].position[2] == 0);

    EXPECT_NEAR(env[1].velocity[0], cns2[0], 1e-5);
    EXPECT_NEAR(env[1].velocity[1], cns2[1], 1e-5);
    EXPECT_NEAR(env[1].velocity[2], cns2[2], 1e-5);

    EXPECT_TRUE(env[1].mass == 5);
    EXPECT_TRUE(env[1].type == 0);
}