#include <gtest/gtest.h>
#include "../src/env/Force.h"
#include "../src/env/Environment.h"

auto grid = md::env::ParticleGrid();
auto particle1= md::env::Particle(0, grid, {1,5,4},{3,3,3},5,0);
auto particle2= md::env::Particle(0, grid,{3,2,1},{0,0,0},5,0);

//check the correctness of InverseSquare calculation
TEST(InverseSquareTest, test) {
    auto function = md::env::InverseSquare();
    EXPECT_NEAR(function(particle1,particle2).operator[](0),-0.484547082626387350678267573713271309978122763782204251405547716,1e-5);
    EXPECT_NEAR( function(particle1,particle2).operator[](1),0.726820623939581026017401360569906964967184145673306377108321573,1e-5);
    EXPECT_NEAR( function(particle1,particle2).operator[](2),0.726820623939581026017401360569906964967184145673306377108321573, 1e-5);
}

//check the correctness of HookesLaw calculation
TEST(HookesLawTest,test ) {
    auto function = md::env::HookesLaw();
    auto result =function(particle1,particle2);
    EXPECT_NEAR(result.operator[](0),-0.2,1e-5);
    EXPECT_NEAR(result.operator[](1),0.3,1e-5);
    EXPECT_NEAR(result.operator[](2),0.3,1e-5);
}

//check the correctness of Lennard_jones calculation
TEST(Lennard_jones,test ) {
    auto function = md::env::LennardJones(1.0,1.0,5);
    auto result =function(particle1,particle2);
    EXPECT_NEAR(result.operator[](0), 2*-0.000102432774875326952280554216925586582064682451855120479006419,1e-5);
    EXPECT_NEAR(result.operator[](1), 3 * 0.000102432774875326952280554216925586582064682451855120479006419,1e-5);
    EXPECT_NEAR(result.operator[](2), 3 * 0.000102432774875326952280554216925586582064682451855120479006419,1e-5);
}

//check the correctness of Lennard_jones calculation when dist_squared > cutoff_radius * cutoff_radius
TEST(Lennard_jones_cutoffradius,test ) {
    auto function = md::env::LennardJones();
    auto result =function(particle1,particle2);
    EXPECT_TRUE(result.operator[](0) == 0);
    EXPECT_TRUE(result.operator[](1) == 0);
    EXPECT_TRUE(result.operator[](2) == 0);
}