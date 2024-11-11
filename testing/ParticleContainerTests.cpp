#include <gtest/gtest.h>
#include "../src/Particle.h"

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