#include "core/StoermerVerlet.h"
#include "io/IOStrategy.h"
#include "utils/Parse.h"
#include "io/Logger/Logger.h"
#include "utils/MaxwellBoltzmannDistribution.h"
#include "demos.h"
using namespace md;

int sim_main(const int argc, char* argv[]) {

    io::Logger::initialize_logger();

    io::ProgramArguments args;
    switch (parse::parse_args(argc, argv, args)) {
        case parse::EXIT:
            exit(0);
        case parse::ERROR:
            exit(-1);
        default:;
    };

    auto writer = create_writer(args.output_baseName, args.output_format, args.override);
    auto checkpoint_writer = io::create_checkpoint_writer();
    Integrator::StoermerVerlet simulator(args.env, std::move(writer), std::move(checkpoint_writer), args.thermostat);

    if (!args.benchmark) {
        simulator.simulate(0, args.duration, args.dt, args.write_freq, args.temp_adj_freq);
    } else {
        simulator.benchmark(0, args.duration, args.dt);
    }
    return 0;
}

using namespace md::env;
int main(const int argc, char* argv[]) {
    Environment env;
    Boundary boundary;
    boundary.origin = {0, 0, 0};
    boundary.extent = {100, 100, 1};

    boundary.set_boundary_rule(PERIODIC);
    env.set_force(LennardJones(1, 1, 0), 0);
    env.set_grid_constant(3);
    env.set_boundary(boundary);
    env.set_dimension(Dimension::TWO);

    env.add_cuboid({30, 30, 0}, {}, {20, 20, 1}, 3, 1);
    env.build();

    Thermostat thermostat(20, 80, 1);
    thermostat.set_initial_temperature(env);
    SPDLOG_INFO("TEMP: {}, soll = 20", env.temperature());

    md::Integrator::StoermerVerlet holding_simulator(env, nullptr, nullptr, thermostat);
    holding_simulator.simulate(0, 1, 0.001, -1, 10);

    SPDLOG_INFO("TEMP: {}, soll = 80", env.temperature(env.average_velocity()));

    return 0;
}