#include <gtest/gtest.h>

#include "../src/core/StoermerVerlet.h"
#include "../src/env/Environment.h"
#include "../src/env/ParticleGrid.h"

md::env::ParticleGrid grid;
md::env::Boundary boundary;
std::vector<md::env::Particle> particle_storage;

md::env::Particle particle1 = md::env::Particle(0, grid, {7.5, 7.5, 0}, {0.0, 0.0, 0.0}, 100.0, 0);
md::env::Particle particle2 = md::env::Particle(0, grid, {4.5, 7.5, 0}, {0.0, 0.0, 0.0}, 1.0, 0);
md::env::Particle particle3 = md::env::Particle(0, grid, {7.5, 4.5, 0}, {0.0, 0.0, 0.0}, 1.0, 0);
md::env::Particle particle4 = md::env::Particle(0, grid, {13.5, 13.5, 0}, {0.0, 0.0, 0.0}, 1.0, 0);

void init_grid() {
    boundary.origin = {0, 0, 0};
    boundary.extent = {15, 15, 1};
    boundary.set_boundary_rule(md::env::BoundaryRule::OUTFLOW);

    particle_storage.emplace_back(particle1);
    particle_storage.emplace_back(particle2);
    particle_storage.emplace_back(particle3);
    particle_storage.emplace_back(particle4);

    grid.build(boundary, 3, particle_storage);
}

// tests if grid is constructed correctly
TEST(LinkedCellsTest, grid_construction_test) {
    init_grid();

    md::env::GridCell cell1 = grid.get_cell({2, 2, 0});
    md::env::GridCell cell2 = grid.get_cell({0, 0, 0});
    md::env::GridCell cell3 = grid.get_cell({0, 4, 0});
    md::env::GridCell cell4 = grid.get_cell({2, 4, 0});

    EXPECT_EQ(cell1.to_string(), "origin:[6, 6, 0] size: [3, 3, 1] type: 769 Particles: 1");
    EXPECT_EQ(cell2.to_string(), "origin:[0, 0, 0] size: [3, 3, 1] type: 929 Particles: 0");
    EXPECT_EQ(cell3.to_string(), "origin:[0, 12, 0] size: [3, 3, 1] type: 865 Particles: 0");
    EXPECT_EQ(cell4.to_string(), "origin:[6, 12, 0] size: [3, 3, 1] type: 833 Particles: 0");
}

// tests if grid pairs are created correctly
TEST(LinkedCellsTest, grid_pair_test) {
    std::vector<md::env::CellPair> cell_pair = grid.linked_cells();
    int i = 0;

    for (auto& pair : cell_pair) {
        for (auto [p1, p2] : pair.particles()) {
            switch (i) {
                case 0:
                    EXPECT_EQ(p1->to_string(),
                              "Particle: X:[7.5, 4.5, 0] v: [0, 0, 0] f: [0, 0, 0] old_f: [0, 0, 0] type: 0 id: 0");
                    EXPECT_EQ(p2->to_string(),
                              "Particle: X:[4.5, 7.5, 0] v: [0, 0, 0] f: [0, 0, 0] old_f: [0, 0, 0] type: 0 id: 0");
                    break;
                case 1:
                    EXPECT_EQ(p1->to_string(),
                              "Particle: X:[7.5, 7.5, 0] v: [0, 0, 0] f: [0, 0, 0] old_f: [0, 0, 0] type: 0 id: 0");
                    EXPECT_EQ(p2->to_string(),
                              "Particle: X:[4.5, 7.5, 0] v: [0, 0, 0] f: [0, 0, 0] old_f: [0, 0, 0] type: 0 id: 0");
                    break;
                case 2:
                    EXPECT_EQ(p1->to_string(),
                              "Particle: X:[7.5, 7.5, 0] v: [0, 0, 0] f: [0, 0, 0] old_f: [0, 0, 0] type: 0 id: 0");
                    EXPECT_EQ(p2->to_string(),
                              "Particle: X:[7.5, 4.5, 0] v: [0, 0, 0] f: [0, 0, 0] old_f: [0, 0, 0] type: 0 id: 0");
                    break;
                default:
                    EXPECT_TRUE(false);
                    break;
            }
            ++i;
        }
    }
}