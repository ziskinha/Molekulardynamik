#include <gtest/gtest.h>
#include "../src/core/StoermerVerlet.h"
#include "../src/env/Environment.h"
#include "../src/env/Force.h"
#include "../src/core/IntegratorBase.h"



//Check correctness of updated values after performing a single simulation step 
TEST(StoermerVerletTest, test){
    md::env::Environment env;
    md::env::Boundary boundary;
    boundary.extent = {10, 10, 10};
    boundary.origin = {0, 0, 0};
    boundary.set_boundary_rule(md::env::BoundaryRule::OUTFLOW);
    env.set_boundary(boundary);
    env.add_particle({1, 5, 4}, {3, 3, 3}, 5, 0);
    env.add_particle({3, 2, 1}, {0, 0, 0}, 5, 0);
    env.set_grid_constant(10);
    env.set_force(md::env::InverseSquare(1,10));
    env.build();

    md::Integrator::StoermerVerlet simulator (env, nullptr);
    simulator.simulate(0, 1.0, 1.0, 1000);

    const auto& afterSimParticle1 = env.operator[](0);
    const auto& afterSimParticle2 = env.operator[](1);

    ASSERT_TRUE(afterSimParticle1.position[0]==4);
    ASSERT_TRUE(afterSimParticle1.position[1]==8);
    ASSERT_TRUE(afterSimParticle1.position[2]==7);

    ASSERT_TRUE(afterSimParticle2.position[0]==3);
    ASSERT_TRUE(afterSimParticle2.position[1]==2);
    ASSERT_TRUE(afterSimParticle2.position[2]==1);

    EXPECT_NEAR(env.operator[](0).force.operator[](0), -0.0400825846562090972409065881321059600046652999133668710917088161, 1e-5);
    EXPECT_NEAR(env.operator[](0).force.operator[](1), -0.2404955079372545834454395287926357600279917994802012265502528966, 1e-5);
    EXPECT_NEAR(env.operator[](0).force.operator[](2), -0.2404955079372545834454395287926357600279917994802012265502528966, 1e-5);

    EXPECT_NEAR(afterSimParticle1.velocity[0],3-0.00400825846562090972409065881321059600046652999133668710917088161,1e-5);
    EXPECT_NEAR(afterSimParticle1.velocity[1],3-0.02404955079372545834454395287926357600279917994802012265502528966,1e-5);
    EXPECT_NEAR(afterSimParticle1.velocity[2],3-0.02404955079372545834454395287926357600279917994802012265502528966,1e-5);
    
    EXPECT_NEAR(env.operator[](1).force.operator[](0), 0.0400825846562090972409065881321059600046652999133668710917088161, 1e-5);
    EXPECT_NEAR(env.operator[](1).force.operator[](1), 0.2404955079372545834454395287926357600279917994802012265502528966, 1e-5);
    EXPECT_NEAR(env.operator[](1).force.operator[](2), 0.2404955079372545834454395287926357600279917994802012265502528966, 1e-5);

    EXPECT_NEAR(afterSimParticle2.velocity[0],0.00400825846562090972409065881321059600046652999133668710917088161,1e-5);
    EXPECT_NEAR(afterSimParticle2.velocity[1],0.02404955079372545834454395287926357600279917994802012265502528966,1e-5);
    EXPECT_NEAR(afterSimParticle2.velocity[2],0.02404955079372545834454395287926357600279917994802012265502528966,1e-5);
}


TEST(StoermerVerletOrbitTest, test){
    md::env::Environment env;

    md::env::Boundary boundary;
    boundary.extent = {15, 15, 10};
    boundary.origin = {-5, -5, -5};
    boundary.set_boundary_rule(md::env::BoundaryRule::OUTFLOW);

    constexpr double G = 1;
    constexpr int R = 1;
    constexpr double M = 1.0;
    constexpr double m = 1e-10;
    constexpr double v = G*M/R;
    constexpr double T = 2* 3.14159265359 * v/R;

    env.set_boundary(boundary);
    env.add_particle({0, R, 0}, {v, 0, 0}, m, 0);
    env.add_particle({0, 0, 0}, {0, 0, 0}, M, 1);
    env.set_grid_constant(30); // Simulator uses direct sum method
    env.set_force(md::env::InverseSquare(G,10));
    env.build();

    md::Integrator::StoermerVerlet simulator (env, nullptr);
    simulator.simulate(0, T, 0.001, 10000);

    const auto p = env[0];

    EXPECT_NEAR(p.position[0], 0, 1e-3 );
    EXPECT_NEAR(p.position[1], R, 1e-3);
    EXPECT_EQ(p.position[2], 0);

    EXPECT_NEAR(p.velocity[0], v, 1e-3);
    EXPECT_NEAR(p.velocity[1], 0, 1e-3);
    EXPECT_EQ(p.velocity[2], 0);

}

TEST(StoermerVerletOrbitTestLinkedCells, test){
    md::env::Environment env;

    md::env::Boundary boundary;
    boundary.extent = {3.6, 3.6, 3};
    boundary.origin = {-1.8, -1.8, 0};
    boundary.set_boundary_rule(md::env::BoundaryRule::OUTFLOW);

    constexpr double G = 1;
    constexpr int R = 1;
    constexpr double M = 1.0;
    constexpr double m = 1e-10;
    constexpr double v = G*M/R;
    constexpr double T = 2* 3.14159265359 * v/R;

    env.set_boundary(boundary);
    env.add_particle({0, R, 0}, {v, 0, 0}, m, 0);
    env.add_particle({0, 0, 0}, {0, 0, 0}, M, 1);
    env.set_grid_constant(1.2); // uses
    env.set_force(md::env::InverseSquare(G,1.2));
    env.build();

    md::Integrator::StoermerVerlet simulator (env, nullptr);
    simulator.simulate(0, T, 0.001, 10000);

    const auto p = env[0];

    EXPECT_NEAR(p.position[0], 0, 1e-3 );
    EXPECT_NEAR(p.position[1], R, 1e-3);
    EXPECT_EQ(p.position[2], 0);

    EXPECT_NEAR(p.velocity[0], v, 1e-3);
    EXPECT_NEAR(p.velocity[1], 0, 1e-3);
    EXPECT_EQ(p.velocity[2], 0);

}

