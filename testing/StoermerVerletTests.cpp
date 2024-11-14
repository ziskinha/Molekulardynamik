#include <gtest/gtest.h>
#include "../src/StoermerVerlet.h"
#include "../src/Particle.h"
#include "../src/force.h"
#include "../src/IntegratorBase.h"


//Check correctness of updated values after performing a single simulation step 
TEST(StoermerVerletTest, test){

auto particle1= md::Particle({1,5,4},{3,3,3},5,0);
auto particle2= md::Particle({3,2,1},{0,0,0},5,0);
auto particleContainer = md::ParticleContainer ({particle1,particle2});
md::Integrator::StoermerVerlet verlet (particleContainer, md::force::inverse_square(1.0),nullptr);
md::Integrator::dummy_run_simulation(verlet, 1.0);

const auto& afterSimParticle1 = particleContainer[0];
const auto& afterSimParticle2 = particleContainer[1];

ASSERT_TRUE(afterSimParticle1.position.operator[](0)==4);
ASSERT_TRUE(afterSimParticle1.position.operator[](1)==8);
ASSERT_TRUE(afterSimParticle1.position.operator[](2)==7);

ASSERT_TRUE(afterSimParticle2.position.operator[](0)==3);
ASSERT_TRUE(afterSimParticle2.position.operator[](1)==2);
ASSERT_TRUE(afterSimParticle2.position.operator[](2)==1);

EXPECT_NEAR(afterSimParticle1.force.operator[](0),-0.0400825846562090972409065881321059600046652999133668710917088161, 1e-5);
EXPECT_NEAR(afterSimParticle1.force.operator[](1), -0.2404955079372545834454395287926357600279917994802012265502528966, 1e-5);
EXPECT_NEAR(afterSimParticle1.force.operator[](2), -0.2404955079372545834454395287926357600279917994802012265502528966, 1e-5);

EXPECT_NEAR(afterSimParticle1.velocity.operator[](0),3-0.00400825846562090972409065881321059600046652999133668710917088161,1e-5);
EXPECT_NEAR(afterSimParticle1.velocity.operator[](1),3-0.02404955079372545834454395287926357600279917994802012265502528966,1e-5);
EXPECT_NEAR(afterSimParticle1.velocity.operator[](2),3-0.02404955079372545834454395287926357600279917994802012265502528966,1e-5);

EXPECT_NEAR(afterSimParticle2.force.operator[](0), 0.0400825846562090972409065881321059600046652999133668710917088161, 1e-5);
EXPECT_NEAR(afterSimParticle2.force.operator[](1), 0.2404955079372545834454395287926357600279917994802012265502528966, 1e-5);
EXPECT_NEAR(afterSimParticle2.force.operator[](2), 0.2404955079372545834454395287926357600279917994802012265502528966, 1e-5);

EXPECT_NEAR(afterSimParticle2.velocity.operator[](0),0.00400825846562090972409065881321059600046652999133668710917088161,1e-5);
EXPECT_NEAR(afterSimParticle2.velocity.operator[](1),0.02404955079372545834454395287926357600279917994802012265502528966,1e-5);
EXPECT_NEAR(afterSimParticle2.velocity.operator[](2),0.02404955079372545834454395287926357600279917994802012265502528966,1e-5);

}

