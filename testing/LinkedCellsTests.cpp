#include <gtest/gtest.h>

#include "../src/core/StoermerVerlet.h"
#include "../src/env/Environment.h"
#include "../src/env/ParticleGrid.h"

md::env::ParticleGrid grid;
md::env::Boundary boundary;
std::vector<md::env::Particle> particle_storage;

auto particle1 = md::env::Particle(0, grid, {7.5, 7.5, 0}, {0.0, 0.0, 0.0}, 100.0, 0);
auto particle2 = md::env::Particle(0, grid, {4.5, 7.5, 0}, {0.0, 0.0, 0.0}, 1.0, 0);
auto particle3 = md::env::Particle(0, grid, {7.5, 4.5, 0}, {0.0, 0.0, 0.0}, 1.0, 0);
auto particle4 = md::env::Particle(0, grid, {13.5, 13.5, 0}, {0.0, 0.0, 0.0}, 1.0, 0);

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

struct SymmetricPairHasher {  // symmetric pair hasher
    template <typename T1, typename T2>
    std::size_t operator()(const std::pair<T1, T2>& key) const {
        std::size_t h1 = std::hash<T1>()(key.first);
        std::size_t h2 = std::hash<T2>()(key.second);
        return h1 ^ h2 ^ ((h1 + h2) << 1);
    }
};

TEST(LinkedCellsTest, no_double_particle_pairs) {
    boundary.origin = {0, 0, 0};
    boundary.extent = {50, 50, 50};
    boundary.set_boundary_rule(md::env::BoundaryRule::PERIODIC);

    md::env::Environment env;
    env.add_cuboid({0.5,0.5,0.5}, {}, {49,49,49}, 0, 1, 0, 3);
    env.set_boundary(boundary);
    env.set_gravity_constant(10);

    for (auto& cell_pair : env.linked_cells()) {
        std::unordered_set<std::pair<int, int>, SymmetricPairHasher> pairs;
        for (auto [p1, p2] : cell_pair.particles()) {
            std::pair<int, int> current_pair{p1->id, p2->id};
            ASSERT_FALSE(pairs.find(current_pair) != pairs.end()) << "Duplicate pair found: {"
                                                                             << current_pair.first << ", "
                                                                             << current_pair.second << "}";
            pairs.insert(current_pair);
        }
    }

}