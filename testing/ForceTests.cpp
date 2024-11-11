#include <gtest/gtest.h>
#include "../src/force.h"

auto particle1= md::Particle({1,5,4},{3,3,3},5,0);
auto particle2= md::Particle({3,2,1},{0,0,0},5,0);

TEST(InverseSquareTest, test) {
    auto function = md::force::inverse_square();
    EXPECT_NEAR(function(particle1,particle2).operator[](0),0.484547082626387350678267573713271309978122763782204251405547716,1e-5);
    EXPECT_NEAR( function(particle1,particle2).operator[](1),-0.726820623939581026017401360569906964967184145673306377108321573,1e-5);
    EXPECT_NEAR( function(particle1,particle2).operator[](2),-0.726820623939581026017401360569906964967184145673306377108321573, 1e-5);
}

TEST(HookesLawTest,test ) {
    auto function = md::force::hookes_law();
    auto result =function(particle1,particle2);
    EXPECT_NEAR(result.operator[](0),0.2,1e-5);
    EXPECT_NEAR(result.operator[](1),-0.3,1e-5);
    EXPECT_NEAR(result.operator[](2),-0.3,1e-5);
}

TEST(Lennard_jones,test ) {
    auto function = md::force::lennard_jones();
    auto result =function(particle1,particle2);
std::cout << "Vector: " << result.operator[](0) << result.operator[](1)   << result.operator[](2)  << std::endl;
    EXPECT_NEAR(result.operator[](0),8*0.0001201130753994197428606661070814094533570211053461018511324858,1e-5);
    EXPECT_NEAR(result.operator[](1),-12 * 0.0001201130753994197428606661070814094533570211053461018511324858,1e-5);
    EXPECT_NEAR(result.operator[](2),-12 * 0.0001201130753994197428606661070814094533570211053461018511324858,1e-5);
}