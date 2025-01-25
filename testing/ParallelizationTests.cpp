#include <gtest/gtest.h>

#include "../src/core/StoermerVerlet/StoermerVerlet.h"
#include "../src/core/StoermerVerlet/StoermerVerletCellLock.h"
#include "../src/core/StoermerVerlet/StoermerVerletSpatialDecomp.h"
#include "../src/env/Boundary.h"
#include "../src/env/Environment.h"
#include "../src/env/Particle.h"
#include "../src/env/ParticleGrid.h"

using namespace md;

void setup(env::Environment &env, bool build_blocks) {
    env::Boundary boundary;
    boundary.set_boundary_rule(env::BoundaryRule::PERIODIC);
    boundary.extent = {10, 10, 10};
    env.set_boundary(boundary);

    env.add_cuboid({0.5, 0.5, 0.5}, {0, 2, 0}, {9, 4, 9}, 1, 1, 0, 0, env::Dimension::THREE, env::Particle::ALIVE);
    env.set_force(env::LennardJones(1, 1.1, 2.5), 0);
    env.add_cuboid({0.5, 4.5, 0.5}, {0, -2, 0}, {9, 4, 9}, 1, 1, 0, 1, env::Dimension::THREE, env::Particle::ALIVE);
    env.set_force(env::LennardJones(1, 1.2, 2.5), 1);

    env.build(build_blocks);
}

// tests if blocks are built correctly and are assigned to the correct set (Spatial decomposition).
TEST(ParallelizationTests, create_blocks_test) {
    env::Boundary boundary_3;
    boundary_3.set_boundary_rule(env::BoundaryRule::OUTFLOW);
    boundary_3.set_boundary_rule(env::BoundaryRule::PERIODIC, env::BoundaryNormal::BOTTOM);
    boundary_3.set_boundary_rule(env::BoundaryRule::PERIODIC, env::BoundaryNormal::TOP);
    boundary_3.extent = {15, 12.5, 5};
    env::ParticleGrid grid;
    std::vector<env::Particle> particles;

    grid.n_threads = 6;
    grid.build(boundary_3, 2.5, particles, true);

    std::vector<std::vector<env::Block>> blocks = grid.block_sets();

    EXPECT_TRUE(blocks[0].size() == 6);
    EXPECT_TRUE(blocks[1].size() == 2);
    EXPECT_TRUE(blocks[2].size() == 2);
    EXPECT_TRUE(blocks[3].size() == 0);

    // normal blocks
    EXPECT_EQ(blocks[0][0].to_string(),
              "origin: [0, 0, 0] extent: [2, 3, 2] idx: [0, 0, 0]\n"
              "cell pairs: [0, 0] [0, 1] [0, 2] [0, 3] [0, 10] [0, 11] [0, 12] [0, 13] [1, 1] "
              "[1, 2] [1, 3] [1, 10] [1, 11] [1, 12] [1, 13] [2, 2] [2, 3] [2, 4] [2, 5] "
              "[2, 10] [2, 11] [2, 12] [2, 13] [2, 14] [2, 15] [3, 3] [3, 4] [3, 5] [3, 10] "
              "[3, 11] [3, 12] [3, 13] [3, 14] [3, 15] [4, 4] [4, 5] [4, 12] [4, 13] [4, 14] "
              "[4, 15] [5, 5] [5, 12] [5, 13] [5, 14] [5, 15] [10, 10] [10, 11] [10, 12] "
              "[10, 13] [11, 11] [11, 12] [11, 13] [12, 12] [12, 13] [12, 14] [12, 15] [13, 13] [13, 14] [13, 15] [14, "
              "14] [14, 15] [15, 15] ");

    EXPECT_EQ(blocks[0][1].to_string(),
              "origin: [0, 3, 0] extent: [2, 2, 2] idx: [0, 1, 0]\n"
              "cell pairs: [6, 6] [6, 7] [6, 8] [6, 9] [6, 16] [6, 17] [6, 18] [6, 19] [7, 7] [7, 8] [7, 9] [7, 16] "
              "[7, 17] [7, 18] [7, 19] [8, 8] [8, 9] [8, 16] [8, 17] [8, 18] [8, 19] [9, 9] [9, 16] [9, 17] [9, 18] "
              "[9, 19] [16, 16] [16, 17] [16, 18] [16, 19] [17, 17] [17, 18] [17, 19] [18, 18] [18, 19] [19, 19] ");

    EXPECT_EQ(blocks[0][2].to_string(),
              "origin: [2, 0, 0] extent: [2, 3, 2] idx: [1, 0, 0]\n"
              "cell pairs: [20, 20] [20, 21] [20, 22] [20, 23] [20, 30] [20, 31] [20, 32] [20, 33] [21, 21] [21, 22] "
              "[21, 23] [21, 30] [21, 31] [21, 32] [21, 33] [22, 22] [22, 23] [22, 24] [22, 25] [22, 30] [22, 31] [22, "
              "32] [22, 33] [22, 34] [22, 35] [23, 23] [23, 24] [23, 25] [23, 30] [23, 31] [23, 32] [23, 33] [23, 34] "
              "[23, 35] [24, 24] [24, 25] [24, 32] [24, 33] [24, 34] [24, 35] [25, 25] [25, 32] [25, 33] [25, 34] [25, "
              "35] [30, 30] [30, 31] [30, 32] [30, 33] [31, 31] [31, 32] [31, 33] [32, 32] [32, 33] [32, 34] [32, 35] "
              "[33, 33] [33, 34] [33, 35] [34, 34] [34, 35] [35, 35] ");

    EXPECT_EQ(blocks[0][3].to_string(),
              "origin: [2, 3, 0] extent: [2, 2, 2] idx: [1, 1, 0]\n"
              "cell pairs: [26, 26] [26, 27] [26, 28] [26, 29] [26, 36] [26, 37] [26, 38] [26, 39] [27, 27] [27, 28] "
              "[27, 29] [27, 36] [27, 37] [27, 38] [27, 39] [28, 28] [28, 29] [28, 36] [28, 37] [28, 38] [28, 39] [29, "
              "29] [29, 36] [29, 37] [29, 38] [29, 39] [36, 36] [36, 37] [36, 38] [36, 39] [37, 37] [37, 38] [37, 39] "
              "[38, 38] [38, 39] [39, 39] ");

    EXPECT_EQ(blocks[0][4].to_string(),
              "origin: [4, 0, 0] extent: [2, 3, 2] idx: [2, 0, 0]\n"
              "cell pairs: [40, 40] [40, 41] [40, 42] [40, 43] [40, 50] [40, 51] [40, 52] [40, 53] [41, 41] [41, 42] "
              "[41, 43] [41, 50] [41, 51] [41, 52] [41, 53] [42, 42] [42, 43] [42, 44] [42, 45] [42, 50] [42, 51] [42, "
              "52] [42, 53] [42, 54] [42, 55] [43, 43] [43, 44] [43, 45] [43, 50] [43, 51] [43, 52] [43, 53] [43, 54] "
              "[43, 55] [44, 44] [44, 45] [44, 52] [44, 53] [44, 54] [44, 55] [45, 45] [45, 52] [45, 53] [45, 54] [45, "
              "55] [50, 50] [50, 51] [50, 52] [50, 53] [51, 51] [51, 52] [51, 53] [52, 52] [52, 53] [52, 54] [52, 55] "
              "[53, 53] [53, 54] [53, 55] [54, 54] [54, 55] [55, 55] ");

    EXPECT_EQ(blocks[0][5].to_string(),
              "origin: [4, 3, 0] extent: [2, 2, 2] idx: [2, 1, 0]\n"
              "cell pairs: [46, 46] [46, 47] [46, 48] [46, 49] [46, 56] [46, 57] [46, 58] [46, 59] [47, 47] [47, 48] "
              "[47, 49] [47, 56] [47, 57] [47, 58] [47, 59] [48, 48] [48, 49] [48, 56] [48, 57] [48, 58] [48, 59] [49, "
              "49] [49, 56] [49, 57] [49, 58] [49, 59] [56, 56] [56, 57] [56, 58] [56, 59] [57, 57] [57, 58] [57, 59] "
              "[58, 58] [58, 59] [59, 59] ");

    // communication_blocks_x
    EXPECT_EQ(blocks[1][0].to_string(),
              "origin: [0, 0, 0] extent: [0, 0, 0] idx: [0, 0, 0]\n"
              "cell pairs: [30, 48] [30, 49] [30, 40] [30, 41] [30, 42] [30, 43] [31, 48] [31, 49] [31, 40] [31, 41] "
              "[31, 42] [31, 43] [32, 40] [32, 41] [32, 42] [32, 43] [32, 44] [32, 45] [33, 40] [33, 41] [33, 42] [33, "
              "43] [33, 44] [33, 45] [34, 42] [34, 43] [34, 44] [34, 45] [34, 46] [34, 47] [35, 42] [35, 43] [35, 44] "
              "[35, 45] [35, 46] [35, 47] [36, 44] [36, 45] [36, 46] [36, 47] [36, 48] [36, 49] [37, 44] [37, 45] [37, "
              "46] [37, 47] [37, 48] [37, 49] [38, 46] [38, 47] [38, 48] [38, 49] [38, 40] [38, 41] [39, 46] [39, 47] "
              "[39, 48] [39, 49] [39, 40] [39, 41] ");

    EXPECT_EQ(blocks[1][1].to_string(),
              "origin: [0, 0, 0] extent: [0, 0, 0] idx: [0, 0, 0]\n"
              "cell pairs: [10, 28] [10, 29] [10, 20] [10, 21] [10, 22] [10, 23] [11, 28] [11, 29] [11, 20] [11, 21] "
              "[11, 22] [11, 23] [12, 20] [12, 21] [12, 22] [12, 23] [12, 24] [12, 25] [13, 20] [13, 21] [13, 22] [13, "
              "23] [13, 24] [13, 25] [14, 22] [14, 23] [14, 24] [14, 25] [14, 26] [14, 27] [15, 22] [15, 23] [15, 24] "
              "[15, 25] [15, 26] [15, 27] [16, 24] [16, 25] [16, 26] [16, 27] [16, 28] [16, 29] [17, 24] [17, 25] [17, "
              "26] [17, 27] [17, 28] [17, 29] [18, 26] [18, 27] [18, 28] [18, 29] [18, 20] [18, 21] [19, 26] [19, 27] "
              "[19, 28] [19, 29] [19, 20] [19, 21] ");

    // communication_blocks_y
    EXPECT_EQ(
        blocks[2][0].to_string(),
        "origin: [0, 0, 0] extent: [0, 0, 0] idx: [0, 0, 0]\n"
        "cell pairs: [0, 8] [0, 9] [0, 18] [0, 19] [1, 8] [1, 9] [1, 18] [1, 19] [10, 18] [10, 19] [11, 18] [11, 19] "
        "[8, 10] [8, 11] [9, 10] [9, 11] [20, 28] [20, 29] [20, 38] [20, 39] [21, 28] [21, 29] [21, 38] [21, 39] [30, "
        "38] [30, 39] [31, 38] [31, 39] [28, 30] [28, 31] [29, 30] [29, 31] [40, 48] [40, 49] [40, 58] [40, 59] [41, "
        "48] [41, 49] [41, 58] [41, 59] [50, 58] [50, 59] [51, 58] [51, 59] [48, 50] [48, 51] [49, 50] [49, 51] ");

    EXPECT_EQ(
        blocks[2][1].to_string(),
        "origin: [0, 0, 0] extent: [0, 0, 0] idx: [0, 0, 0]\n"
        "cell pairs: [4, 6] [4, 7] [4, 16] [4, 17] [5, 6] [5, 7] [5, 16] [5, 17] [14, 16] [14, 17] [15, 16] [15, 17] "
        "[6, 14] [6, 15] [7, 14] [7, 15] [24, 26] [24, 27] [24, 36] [24, 37] [25, 26] [25, 27] [25, 36] [25, 37] [34, "
        "36] [34, 37] [35, 36] [35, 37] [26, 34] [26, 35] [27, 34] [27, 35] [44, 46] [44, 47] [44, 56] [44, 57] [45, "
        "46] [45, 47] [45, 56] [45, 57] [54, 56] [54, 57] [55, 56] [55, 57] [46, 54] [46, 55] [47, 54] [47, 55] ");
}

// tests if the force and velocity calculations are correct with the cell lock strategy.
TEST(ParallelizationTest, cell_lock_test) {
    env::Environment env_without_1;
    env::Environment env_cell_lock;

    setup(env_without_1, false);
    setup(env_cell_lock, false);

    Integrator::StoermerVerlet simulator_without_1(env_without_1);
    Integrator::StoermerVerletCellLock simulator_cell_lock(env_cell_lock);

    simulator_without_1.simulate(0, 0.05, 0.0005);
    simulator_cell_lock.simulate(0, 0.05, 0.0005);

    for (size_t i = 0; i < env_without_1.size(); i++) {
        EXPECT_NEAR(env_without_1[i].force[0], env_cell_lock[i].force[0], 1e-10);
        EXPECT_NEAR(env_without_1[i].force[1], env_cell_lock[i].force[1], 1e-10);
        EXPECT_NEAR(env_without_1[i].force[2], env_cell_lock[i].force[2], 1e-10);
    }

    for (size_t i = 0; i < env_without_1.size(); i++) {
        EXPECT_NEAR(env_without_1[i].velocity[0], env_cell_lock[i].velocity[0], 1e-10);
        EXPECT_NEAR(env_without_1[i].velocity[1], env_cell_lock[i].velocity[1], 1e-10);
        EXPECT_NEAR(env_without_1[i].velocity[2], env_cell_lock[i].velocity[2], 1e-10);
    }
}


// tests if the force and velocity calculations are correct with the spatial decomposition strategy.
TEST(ParallelizationTest, spatial_decomposition_test) {
    env::Environment env_without_2;
    env::Environment env_spatial;

    setup(env_without_2, false);
    setup(env_spatial, true);

    Integrator::StoermerVerlet simulator_without_2(env_without_2);
    Integrator::StoermerVerletSpatialDecomp simulator_spatial(env_spatial);

    simulator_without_2.simulate(0, 0.05, 0.0005);
    simulator_spatial.simulate(0, 0.05, 0.0005);

    for (size_t i = 0; i < env_without_2.size(); i++) {
        EXPECT_NEAR(env_without_2[i].force[0], env_spatial[i].force[0], 1e-10);
        EXPECT_NEAR(env_without_2[i].force[1], env_spatial[i].force[1], 1e-10);
        EXPECT_NEAR(env_without_2[i].force[2], env_spatial[i].force[2], 1e-10);
    }

    for (size_t i = 0; i < env_without_2.size(); i++) {
        EXPECT_NEAR(env_without_2[i].velocity[0], env_spatial[i].velocity[0], 1e-10);
        EXPECT_NEAR(env_without_2[i].velocity[1], env_spatial[i].velocity[1], 1e-10);
        EXPECT_NEAR(env_without_2[i].velocity[2], env_spatial[i].velocity[2], 1e-10);
    }
}