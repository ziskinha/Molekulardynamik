#include <gtest/gtest.h>
#include "../src/core/StoermerVerlet.h"
#include "../src/env/Environment.h"
#include "../src/env/Force.h"
#include "../src/core/IntegratorBase.h"

void build_env(md::env::Environment& env) {
    md::env::Boundary boundary;
    boundary.extent = {10, 10, 1};
    boundary.origin = {0, 0, 0};
    boundary.set_boundary_rule(md::env::BoundaryRule::OUTFLOW);
    env.set_boundary(boundary);
    env.add_particle({1, 5, 0}, {3, 3, 3}, 5, 0);
    env.add_particle({3, 2, 1}, {0, 0, 0}, 5, 0);
    env.set_grid_constant(1);
    env.set_force(md::env::InverseSquare(1,10));
    env.build();
}


//Check correctness of updated values after performing a single simulation step 
TEST(StoermerVerletTest, test){
    md::env::Environment env;
    build_env(env);

    md::Integrator::StoermerVerlet simulator (env, NULL);
    simulator.simulate_without_writer(0, 1.0, 1.0);

    const auto& afterSimParticle1 = env.operator[](0);
    const auto& afterSimParticle2 = env.operator[](1);

    ASSERT_TRUE(afterSimParticle1.position[0]==4);
    ASSERT_TRUE(afterSimParticle1.position[1]==8);
    ASSERT_TRUE(afterSimParticle1.position[2]==3);

    ASSERT_TRUE(afterSimParticle2.position[0]==3);
    ASSERT_TRUE(afterSimParticle2.position[1]==2);
    ASSERT_TRUE(afterSimParticle2.position[2]==1);

    EXPECT_NEAR(env.operator[](0).force.operator[](0),0, 1e-5);
    EXPECT_NEAR(env.operator[](0).force.operator[](1), 0, 1e-5);
    EXPECT_NEAR(env.operator[](0).force.operator[](2), 0, 1e-5);

    EXPECT_NEAR(afterSimParticle1.velocity[0],3,1e-5);
    EXPECT_NEAR(afterSimParticle1.velocity[1],3,1e-5);
    EXPECT_NEAR(afterSimParticle1.velocity[2],3,1e-5);
    
    EXPECT_NEAR(env.operator[](1).force.operator[](0), 0, 1e-5);
    EXPECT_NEAR(env.operator[](1).force.operator[](1), 0, 1e-5);
    EXPECT_NEAR(env.operator[](1).force.operator[](2), 0, 1e-5);

    EXPECT_NEAR(afterSimParticle2.velocity[0],0,1e-5);
    EXPECT_NEAR(afterSimParticle2.velocity[1],0,1e-5);
    EXPECT_NEAR(afterSimParticle2.velocity[2],0,1e-5);
}

