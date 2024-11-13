#include <gtest/gtest.h>
#include "../src/force.h"

auto particle1= md::Particle({1,5,4},{3,3,3},5,0);
auto particle2= md::Particle({3,2,1},{0,0,0},5,0);

//check the correctness of InverseSquare calculation
TEST(InverseSquareTest, test) {
    auto function = md::force::inverse_square();
    EXPECT_NEAR(function(particle1,particle2).operator[](0),-0.484547082626387350678267573713271309978122763782204251405547716,1e-5);
    EXPECT_NEAR( function(particle1,particle2).operator[](1),0.726820623939581026017401360569906964967184145673306377108321573,1e-5);
    EXPECT_NEAR( function(particle1,particle2).operator[](2),0.726820623939581026017401360569906964967184145673306377108321573, 1e-5);
}

//check the correctness of HookesLaw calculation
TEST(HookesLawTest,test ) {
    auto function = md::force::hookes_law();
    auto result =function(particle1,particle2);
    EXPECT_NEAR(result.operator[](0),-0.2,1e-5);
    EXPECT_NEAR(result.operator[](1),0.3,1e-5);
    EXPECT_NEAR(result.operator[](2),0.3,1e-5);
}

//check the correctness of Lennard_jones calculation
TEST(Lennard_jones,test ) {
    auto function = md::force::lennard_jones();
    auto result =function(particle1,particle2);
std::cout << "Vector: " << result.operator[](0) << result.operator[](1)   << result.operator[](2)  << std::endl;
    EXPECT_NEAR(result.operator[](0),2*-0.000102432774875326952280554216925586582064682451855120479006419,1e-5);
    EXPECT_NEAR(result.operator[](1),3 * 0.000102432774875326952280554216925586582064682451855120479006419,1e-5);
    EXPECT_NEAR(result.operator[](2),3 * 0.000102432774875326952280554216925586582064682451855120479006419,1e-5);
}