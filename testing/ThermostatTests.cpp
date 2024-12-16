#include <gtest/gtest.h>

#include "env/Thermostat.h"
#include "env/Environment.h"
#include "core/StoermerVerlet.h"
#include "utils/MaxwellBoltzmannDistribution.h"

void setup(md::env::Environment &env) {
    md::env::Boundary boundary;
    boundary.origin = {0, 0, 0};
    boundary.extent = {100, 100, 1};
    boundary.set_boundary_rule(md::env::BoundaryRule::VELOCITY_REFLECTION);
    env.add_particle({5, 5, 0}, {0, 0, 0}, 1, 0);
    env.set_force(md::env::LennardJones(1, 1, 2.5), 0);
    env.set_grid_constant(100);
    env.set_boundary(boundary);
    env.build();
    env.build();
}

// tests if the velocity of particles stays constant with no change in temperature.
TEST(ThermostatTests, holding_temperature_test) {
    md::env::Environment env;
    setup(env);

    md::env::Thermostat thermostat;
    thermostat.init(40, 40, 10);
    thermostat.set_initial_temperature(env);

    md::vec3 expected_velocity = env.operator[](0).velocity;

    md::Integrator::StoermerVerlet simulator(env);
    simulator.simulate(0, 2, 0.001, 100000, 10);

    EXPECT_NEAR(expected_velocity[0], env.operator[](0).velocity[0], 0.0001);
    EXPECT_NEAR(expected_velocity[1], env.operator[](0).velocity[1], 0.0001);
    EXPECT_NEAR(expected_velocity[2], env.operator[](0).velocity[2], 0.0001);
}

// tests if the velocity of particles adjusts correctly during cooling.
TEST(ThermostatTest, cooling_test) {
    md::env::Environment env;
    setup(env);

    md::env::Thermostat thermostat;
    thermostat.init(40, 30, -1);
    thermostat.set_initial_temperature(env);

    double beta = sqrt(30/40);
    md::vec3 expected_velocity = beta * env.operator[](0).velocity;

    md::Integrator::StoermerVerlet simulator(env);
    simulator.simulate(0, 1, 1, 1, 1);

    EXPECT_NEAR(expected_velocity[0], env.operator[](0).velocity[0], 0.0001);
    EXPECT_NEAR(expected_velocity[1], env.operator[](0).velocity[1], 0.0001);
    EXPECT_NEAR(expected_velocity[2], env.operator[](0).velocity[2], 0.0001);
}

// tests if the velocity of particles adjusts correctly during heating.
TEST(ThermostatTest, heating_test) {
    md::env::Environment env;
    setup(env);

    md::env::Thermostat thermostat;
    thermostat.init(40, 50, -1);
    thermostat.set_initial_temperature(env);

    double beta = sqrt(50/40);
    md::vec3 expected_velocity = beta * env.operator[](0).velocity;

    md::Integrator::StoermerVerlet simulator(env);
    simulator.simulate(0, 1, 1, 1, 1);

    EXPECT_NEAR(expected_velocity[0], env.operator[](0).velocity[0], 0.0001);
    EXPECT_NEAR(expected_velocity[1], env.operator[](0).velocity[1], 0.0001);
    EXPECT_NEAR(expected_velocity[2], env.operator[](0).velocity[2], 0.0001);
}
