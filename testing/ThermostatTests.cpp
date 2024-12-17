#include <gtest/gtest.h>

#include "env/Thermostat.h"
#include "env/Environment.h"
#include "core/StoermerVerlet.h"

#include <iostream>

void setup(md::env::Environment &env) {
    // set up the environment
    md::env::Boundary boundary;
    boundary.origin = {0, 0, 0};
    boundary.extent = {100, 100, 1};
    boundary.set_boundary_rule(md::env::BoundaryRule::PERIODIC);
    boundary.set_boundary_rule(md::env::BoundaryRule::OUTFLOW, md::env::BoundaryNormal::FRONT);
    boundary.set_boundary_rule(md::env::BoundaryRule::OUTFLOW, md::env::BoundaryNormal::BACK);
    env.add_cuboid({40, 40, 0}, {0, 0, 0}, {2, 2, 1}, 0, 1, 1, 0);
    env.set_force(md::env::LennardJones(1, 1, 2.5), 0);
    env.set_grid_constant(20);
    env.set_boundary(boundary);
    env.build();

    // regulate temperature to 40
    md::env::Thermostat thermostat(40, 40, 0.1);
    thermostat.set_initial_temperature(env);
    md::Integrator::StoermerVerlet simulator(env, nullptr, nullptr, thermostat);
    simulator.simulate(0, 0.1, 0.001, 10, 1);
}

// tests if the velocity of particles stays constant with no change in temperature.
TEST(ThermostatTests, holding_temperature_test) {
    md::env::Environment env;
    setup(env);

    // initial temp: 40, target temp: 40, deltaT = 0.1
    md::env::Thermostat holding_thermostat(40, 40, 0.1);
    md::Integrator::StoermerVerlet holding_simulator(env, nullptr, nullptr, holding_thermostat);

    // v <- beta * v (beta = 1)
    md::vec3 expected_velocity = env.operator[](0).velocity;
    holding_simulator.simulate(0, 3, 0.001, 10, 100);

    EXPECT_NEAR(expected_velocity[0], env.operator[](0).velocity[0], 0.1);
    EXPECT_NEAR(expected_velocity[1], env.operator[](0).velocity[1], 0.1);
    EXPECT_NEAR(expected_velocity[2], env.operator[](0).velocity[2], 0.1);

    EXPECT_TRUE(env.temperature() == 40.00);
}

// tests if the velocity of particles adjusts correctly during cooling.
TEST(ThermostatTest, cooling_test) {
    md::env::Environment env;
    setup(env);

    // initial temp: 40, target temp: 30, deltaT = 0.25
    md::env::Thermostat cooling_thermostat(40, 30, 0.25);
    md::Integrator::StoermerVerlet cooling_simulator(env, nullptr, nullptr, cooling_thermostat);

    // v <- beta * v
    double beta = sqrt(30.0 / 40.0);
    md::vec3 expected_velocity = beta * env.operator[](0).velocity;

    cooling_simulator.simulate(0, 5, 0.001, 10, 10);

    EXPECT_NEAR(expected_velocity[0], env.operator[](0).velocity[0], 0.1);
    EXPECT_NEAR(expected_velocity[1], env.operator[](0).velocity[1], 0.1);
    EXPECT_NEAR(expected_velocity[2], env.operator[](0).velocity[2], 0.1);

    EXPECT_TRUE(env.temperature() == 30.00);
}

// tests if the velocity of particles adjusts correctly during heating.
TEST(ThermostatTest, heating_test) {
    md::env::Environment env;
    setup(env);

    // initial temp: 40, target temp: 50, deltaT = 0.25
    md::env::Thermostat heating_thermostat(40, 50, 0.25);
    md::Integrator::StoermerVerlet heating_simulator(env, nullptr, nullptr, heating_thermostat);

    // v <- beta * v
    double beta = sqrt(50.0 / 40.0);
    md::vec3 expected_velocity = beta * env.operator[](0).velocity;

    heating_simulator.simulate(0, 5, 0.001, 10, 100);

    EXPECT_NEAR(expected_velocity[0], env.operator[](0).velocity[0], 0.1);
    EXPECT_NEAR(expected_velocity[1], env.operator[](0).velocity[1], 0.1);
    EXPECT_NEAR(expected_velocity[2], env.operator[](0).velocity[2], 0.1);

    EXPECT_TRUE(env.temperature() == 50.00);
}
