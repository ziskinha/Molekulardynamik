#include <gtest/gtest.h>
#include "../src/Environment.h"

auto particle1= md::Particle({1,5,4},{3,3,3},5,0);
auto particle2= md::Particle({3,2,1},{0,0,0},5,0);
auto particle3= md::Particle({10,-1,4},{5,0,0},10,0);

auto particle_container = md::ParticleContainer({particle1,particle2,particle3});

//check if the size() method works as it should
TEST(testSize, test) {
    EXPECT_TRUE(particle_container.size()==3);
}
//check if container returns correct particle from the container
TEST(testIndex, test) {
    EXPECT_TRUE(particle_container.operator[](1)==particle2);
}
//check if iterator begin() returns correct particle
TEST(testIteratorBegin, test) {
    EXPECT_TRUE((particle_container.begin().operator[](0))==particle1);
}
//check if the constructor returns a copy of a particle
TEST(testParticleCopyConstructors,test) {
    md::Particle dummy = md::Particle(particle1);
    EXPECT_TRUE(dummy.force==particle1.force
        && dummy.position==particle1.position
        && dummy.velocity==particle1.velocity
        && dummy.type==particle1.type
        && dummy.mass==particle1.mass);
}
//check if new particle can be added to the container
TEST(testParicleContainerAdd,test){
auto dummyParticle= md::Particle({7,7,7},{3,2,1},1,0);
particle_container.add_particles({dummyParticle});
ASSERT_TRUE(particle_container.size()==4);

EXPECT_TRUE(particle_container.operator[](3).position[0]==7);
EXPECT_TRUE(particle_container.operator[](3).position[1]==7);
EXPECT_TRUE(particle_container.operator[](3).position[2]==7);

EXPECT_TRUE(particle_container.operator[](3).velocity[0]==3);
EXPECT_TRUE(particle_container.operator[](3).velocity[1]==2);
EXPECT_TRUE(particle_container.operator[](3).velocity[2]==1);

EXPECT_TRUE(particle_container.operator[](3).mass==1);
EXPECT_TRUE(particle_container.operator[](3).type==0);
}
auto particle_container2 = md::ParticleContainer({particle1});

//test for checking if adding a cuboid succeeds
TEST(testParticleContainerAddCuboid,test){
    
particle_container2.add_cuboid({0,0,0},{1,1,1},{2,1,1},2,1,5,2);
resetRandomEngine();
md::vec3 bltzm= maxwellBoltzmannDistributedVelocity(2, 2);
md::vec3 cns1 = {bltzm[0] +1,bltzm[1] +1,bltzm[2] +1};
bltzm= maxwellBoltzmannDistributedVelocity(2, 2);
md::vec3 cns2 = {bltzm[0] +1,bltzm[1] +1,bltzm[2] +1};

ASSERT_TRUE(particle_container2.size()==3);
EXPECT_TRUE(particle_container2.operator[](1).position[0]==0);
EXPECT_TRUE(particle_container2.operator[](1).position[1]==0);
EXPECT_TRUE(particle_container2.operator[](1).position[2]==0);

EXPECT_NEAR(particle_container2.operator[](1).velocity[0],cns1[0],1e-5);
EXPECT_NEAR(particle_container2.operator[](1).velocity[1],cns1[1],1e-5);
EXPECT_NEAR(particle_container2.operator[](1).velocity[2],cns1[2],1e-5);

EXPECT_TRUE(particle_container2.operator[](1).mass==5);
EXPECT_TRUE(particle_container2.operator[](1).type==0);


EXPECT_TRUE(particle_container2.operator[](2).position[0]==1);
EXPECT_TRUE(particle_container2.operator[](2).position[1]==0);
EXPECT_TRUE(particle_container2.operator[](2).position[2]==0);

EXPECT_NEAR(particle_container2.operator[](2).velocity[0],cns2[0],1e-5);
EXPECT_NEAR(particle_container2.operator[](2).velocity[1],cns2[1],1e-5);
EXPECT_NEAR(particle_container2.operator[](2).velocity[2],cns2[2],1e-5);

EXPECT_TRUE(particle_container2.operator[](2).mass==5);
EXPECT_TRUE(particle_container2.operator[](2).type==0);

EXPECT_TRUE(particle_container2.operator[](0).position[0]==1);
EXPECT_TRUE(particle_container2.operator[](0).position[1]==5);
EXPECT_TRUE(particle_container2.operator[](0).position[2]==4);

EXPECT_TRUE(particle_container2.operator[](0).velocity[0]==3);
EXPECT_TRUE(particle_container2.operator[](0).velocity[1]==3);
EXPECT_TRUE(particle_container2.operator[](0).velocity[2]==3);

EXPECT_TRUE(particle_container2.operator[](0).mass==5);
EXPECT_TRUE(particle_container2.operator[](0).type==0);
}