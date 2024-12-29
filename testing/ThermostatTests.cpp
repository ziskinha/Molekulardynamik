#include <gtest/gtest.h>

#include "effects/Thermostat.h"
#include "env/Environment.h"
#include "core/StoermerVerlet.h"

using namespace md::env;

void setup(Environment & env) {
    // set up the environment
    Boundary boundary;
    boundary.origin = {0, 0, 0};
    boundary.extent = {100, 100, 1};

    boundary.set_boundary_rule(PERIODIC);
    env.set_force(LennardJones(1, 1, 0), 0);
    env.set_grid_constant(3);
    env.set_boundary(boundary);
    env.set_dimension(Dimension::TWO);
}

TEST(ThermostatTests, initial_temperature_test1) {
    Environment env;
    setup(env);

    env.add_particle({30, 10,0}, {-1, 0, 0}, 1);
    env.add_particle({70, 10,0}, {1, 0, 0}, 1);
    env.add_particle({30, 90,0}, {-1, 0, 0}, 1);
    env.add_particle({70, 90,0}, {1, 0, 0}, 1);
    env.build();

    ASSERT_EQ(env.average_velocity(), md::vec3{});
    ASSERT_EQ(env.temperature(), 0.5);
}

TEST(ThermostatTests, initial_temperature_test2) {
    Environment env;
    setup(env);

    env.add_particle({30, 10,0}, {1, 0, 0}, 1);
    env.add_particle({70, 10,0}, {1, 0, 0}, 1);
    env.add_particle({30, 90,0}, {1, 0, 0}, 1);
    env.add_particle({70, 90,0}, {1, 0, 0}, 1);
    env.build();
    ASSERT_EQ(env.average_velocity()[0], 1.0);
    ASSERT_EQ(env.average_velocity()[1], 0.0);
    ASSERT_EQ(env.average_velocity()[2], 0.0);

    ASSERT_EQ(env.temperature({1,0,0}), 0.0);
}

TEST(ThermostatTests, initial_temperature_test3) {
    Environment env;
    setup(env);

    env.add_particle({30, 10,0}, {2, 0, 0}, 10);
    env.add_particle({70, 90,0}, {-2, 0, 0}, 10);
    env.build();

    ASSERT_EQ(env.average_velocity()[0], 0.0);
    ASSERT_EQ(env.average_velocity()[1], 0.0);
    ASSERT_EQ(env.average_velocity()[2], 0.0);

    ASSERT_EQ(env.temperature(), 20);
}


// tests if the velocity of particles stays constant with no change in temperature.
TEST(ThermostatTests, holding_temperature_test) {
    Environment env;
    setup(env);

    env.add_particle({30, 10,0}, {2, 0, 0}, 10);
    env.add_particle({70, 90,0}, {-2, 0, 0}, 10);
    env.build();

    Thermostat holding_thermostat(20, 20, 0.1);
    md::Integrator::StoermerVerlet holding_simulator(env, nullptr, nullptr, holding_thermostat);

    const md::vec3 expected_velocity1 = env[0].velocity;
    const md::vec3 expected_velocity2 = env[1].velocity;

    holding_simulator.simulate(0, 3, 0.001, -1, 10);

    EXPECT_NEAR(expected_velocity1[0], env[0].velocity[0], 0.01);
    EXPECT_NEAR(expected_velocity1[1], env[0].velocity[1], 0.01);
    EXPECT_NEAR(expected_velocity1[2], env[0].velocity[2], 0.01);

    EXPECT_NEAR(expected_velocity2[0], env[1].velocity[0], 0.01);
    EXPECT_NEAR(expected_velocity2[1], env[1].velocity[1], 0.01);
    EXPECT_NEAR(expected_velocity2[2], env[1].velocity[2], 0.01);

    EXPECT_TRUE(env.temperature() == 20.00);
}

TEST(ThermostatTests, holding_temperature_test_large) {
    Environment env;
    setup(env);

    env.add_cuboid({30,30,0}, {}, {20,20,1}, 3, 1);
    env.build();

    Thermostat thermostat(20, 20, 0.1);
    thermostat.set_initial_temperature(env);
    EXPECT_NEAR(env.temperature(), 20.0, 1);

    md::Integrator::StoermerVerlet holding_simulator(env, nullptr, nullptr, thermostat);
    holding_simulator.simulate(0, 1, 0.001, -1, 10);

    EXPECT_NEAR(env.temperature(env.average_velocity()), 20.0, 0.01);
}

// tests if the velocity of particles adjusts correctly during cooling.
TEST(ThermostatTest, cooling_test) {
    Environment env;
    setup(env);

    env.add_particle({30, 10,0}, {2, 0, 0}, 10);
    env.add_particle({70, 90,0}, {-2, 0, 0}, 10);
    env.build();

    Thermostat cooling_thermostat(20, 5, 0.1);
    md::Integrator::StoermerVerlet holding_simulator(env, nullptr, nullptr, cooling_thermostat);

    const md::vec3 expected_velocity1 = 0.5 * env[0].velocity;
    const md::vec3 expected_velocity2 = 0.5 * env[1].velocity;

    holding_simulator.simulate(0, 3, 0.001, -1, 10);

    EXPECT_NEAR(expected_velocity1[0], env[0].velocity[0], 0.01);
    EXPECT_NEAR(expected_velocity1[1], env[0].velocity[1], 0.01);
    EXPECT_NEAR(expected_velocity1[2], env[0].velocity[2], 0.01);

    EXPECT_NEAR(expected_velocity2[0], env[1].velocity[0], 0.01);
    EXPECT_NEAR(expected_velocity2[1], env[1].velocity[1], 0.01);
    EXPECT_NEAR(expected_velocity2[2], env[1].velocity[2], 0.01);

    EXPECT_TRUE(env.temperature() == 5);
}

TEST(ThermostatTests, colling_temperature_test_large) {
    Environment env;
    setup(env);

    env.add_cuboid({30,30,0}, {}, {20,20,1}, 3, 1);
    env.build();

    Thermostat thermostat(20, 5, 1);
    thermostat.set_initial_temperature(env);
    EXPECT_NEAR(env.temperature(env.average_velocity()), 20, 1);

    md::Integrator::StoermerVerlet holding_simulator(env, nullptr, nullptr, thermostat);
    holding_simulator.simulate(0, 1, 0.001, -1, 10);

    EXPECT_NEAR(env.temperature(env.average_velocity()), 5.0, 0.01);
}


// tests if the velocity of particles adjusts correctly during heating.
TEST(ThermostatTest, heating_test) {
    Environment env;
    setup(env);

    env.add_particle({30, 10,0}, {2, 0, 0}, 10);
    env.add_particle({70, 90,0}, {-2, 0, 0}, 10);
    env.build();

    Thermostat cooling_thermostat(20, 80, 1);
    md::Integrator::StoermerVerlet holding_simulator(env, nullptr, nullptr, cooling_thermostat);

    const md::vec3 expected_velocity1 = 2 * env[0].velocity;
    const md::vec3 expected_velocity2 = 2 * env[1].velocity;

    holding_simulator.simulate(0, 3, 0.001, -1, 10);

    EXPECT_NEAR(expected_velocity1[0], env[0].velocity[0], 0.01);
    EXPECT_NEAR(expected_velocity1[1], env[0].velocity[1], 0.01);
    EXPECT_NEAR(expected_velocity1[2], env[0].velocity[2], 0.01);

    EXPECT_NEAR(expected_velocity2[0], env[1].velocity[0], 0.01);
    EXPECT_NEAR(expected_velocity2[1], env[1].velocity[1], 0.01);
    EXPECT_NEAR(expected_velocity2[2], env[1].velocity[2], 0.01);

    EXPECT_NEAR(env.temperature(), 80.00, 0.01);
}


TEST(ThermostatTests, heating_temperature_test_large) {
    Environment env;
    setup(env);

    env.add_cuboid({30,30,0}, {}, {20,20,1}, 3, 1);
    env.build();

    Thermostat thermostat(20, 80, 1);
    thermostat.set_initial_temperature(env);
    EXPECT_NEAR(env.temperature(), 20, 1);

    md::Integrator::StoermerVerlet holding_simulator(env, nullptr, nullptr, thermostat);
    holding_simulator.simulate(0, 1, 0.001, -1, 10);

    EXPECT_NEAR(env.temperature(env.average_velocity()), 80, 0.01);
}

