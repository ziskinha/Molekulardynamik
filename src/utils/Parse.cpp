#include "Parse.h"

#include <algorithm>
#include <fstream>
#include <string>
#include <vector>

#include "io/input/xml/molSimSchema.hxx"

#define RETURN_PARSE_ERROR(err_msg)                                                \
    SPDLOG_ERROR(err_msg);                                                         \
    SPDLOG_ERROR("For help, run the program with ./MolSim -h or ./MolSim --help"); \
    return ERROR;

namespace md::parse {
    void displayHelp() {
        SPDLOG_INFO(
            "Displaying help information.\n"
            "Usage:\n"
            "  ./MolSim <xml_file> <output_format>\n"
            "  ./MolSim -h | --help\n"
            "  ./MolSim -f\n"
            "  ./MolSim -b\n"
            "Arguments:\n"
            "  xml_file         file with input parameters important for the simulation. For more information about "
            "the structure see ../io/molSimSchema.xsd\n"
            "  output_format    Output format: either 'XYZ' or 'VTK'.\n\n"
            "Flags:\n"
            "  -h, --help       Show this help message and exit.\n"
            "  -f               Delete all contents of the output folder before writing.\n"
            "  -b               Benchmark the simulation (output_format and output_folder optional).");
    }

    ParseStatus parse_args(int argc, char** argv, ProgramArguments& args) {
        SPDLOG_INFO("Start parsing arguments");
        std::vector<std::string> arguments(argv, argv + argc);

        // split arguments into flags & parameters
        std::vector<std::string> parameters;
        std::vector<std::string> flags;
        std::partition_copy(arguments.begin(), arguments.end(), std::back_inserter(flags),
                            std::back_inserter(parameters),
                            [](const std::string& arg) { return !arg.empty() && arg[0] == '-'; });

        auto flag_exists = [&](const std::string& option) -> bool {
            return std::find(flags.begin(), flags.end(), option) != flags.end();
        };

        // Check for help flag
        if (flag_exists("-h") || flag_exists("--help")) {
            displayHelp();
            return EXIT;
        }

        // check number of arguments
        // if -b set, expecting at least 2 arguments (filename, xml file)
        // else expecting 3 arguments (filename, xml file, output_format)
        if ((flag_exists("-b") && parameters.size() < 1) || parameters.size() < 2) {
            RETURN_PARSE_ERROR(
                fmt::format("Error: Not enough arguments provided. Received {} arguments.", parameters.size()));
        }

        // set options
        args.benchmark = flag_exists("-b");
        args.override = flag_exists("-f");

        std::ifstream file(arguments[1]);

        try {
            if (!file.is_open()) {
                RETURN_PARSE_ERROR(fmt::format("Unable to open xml file: {}", arguments[1]));
            }

            auto simulation = simulation_(file, xml_schema::flags::dont_validate);
            for (const auto& particle : simulation.get()->particles()) {
                args.env.add_particle({particle.x(), particle.y(), particle.z()},
                                      {particle.vel1(), particle.vel2(), particle.vel3()}, particle.mass(), 0);
            }

            for (const auto& cuboid : simulation.get()->cuboids()) {
                args.env.add_cuboid({cuboid.x(), cuboid.y(), cuboid.z()}, {cuboid.vel1(), cuboid.vel2(), cuboid.vel3()},
                                    {40, 8, 1}, cuboid.thermal_v(), cuboid.width(), cuboid.mass(), cuboid.dimension(),
                                    0);
            }

            for (const auto& sphere : simulation.get()->spheres()) {
                args.env.add_sphere({sphere.x(), sphere.y(), sphere.z()}, {sphere.vel1(), sphere.vel2(), sphere.vel3()},
                                    sphere.thermal_v(), sphere.radius(), sphere.width(), sphere.mass(),
                                    sphere.dimension(), 0);
            }

            args.output_baseName = simulation.get()->output().baseName();
            args.duration = simulation.get()->parameters().tEnd();
            args.dt = simulation.get()->parameters().deltaT();
            std::string types;
            env::Boundary boundary;
            boundary.extent = {simulation.get()->Boundary().EXTENT_WIDTH().get(),
                               simulation.get()->Boundary().EXTENT_HEIGHT().get(),
                               simulation.get()->Boundary().EXTENT_DEPTH().get()};
            boundary.origin = {simulation.get()->Boundary().CENTER_BOUNDARY_ORIGINX().get(),
                               simulation.get()->Boundary().CENTER_BOUNDARY_ORIGINY().get(),
                               simulation.get()->Boundary().CENTER_BOUNDARY_ORIGINZ().get()};
            types = simulation.get()->Boundary().typeFRONT();

            if (types == "OUTFLOW") {
                boundary.set_boundary_rule(md::env::BoundaryRule::OUTFLOW, md::env::BoundaryNormal::FRONT);

            } else if (types == "VELOCITY_REFLECTION") {
                boundary.set_boundary_rule(md::env::BoundaryRule::VELOCITY_REFLECTION, md::env::BoundaryNormal::FRONT);

            } else if (types == "REPULSIVE_FORCE") {
                boundary.set_boundary_rule(md::env::BoundaryRule::REPULSIVE_FORCE, md::env::BoundaryNormal::FRONT);

            } else {
                boundary.set_boundary_rule(md::env::BoundaryRule::PERIODIC, md::env::BoundaryNormal::FRONT);
            }

            types = simulation.get()->Boundary().typeBACK();

            if (types == "OUTFLOW") {
                boundary.set_boundary_rule(md::env::BoundaryRule::OUTFLOW, md::env::BoundaryNormal::BACK);

            } else if (types == "VELOCITY_REFLECTION") {
                boundary.set_boundary_rule(md::env::BoundaryRule::VELOCITY_REFLECTION, md::env::BoundaryNormal::BACK);

            } else if (types == "REPULSIVE_FORCE") {
                boundary.set_boundary_rule(md::env::BoundaryRule::REPULSIVE_FORCE, md::env::BoundaryNormal::BACK);

            } else {
                boundary.set_boundary_rule(md::env::BoundaryRule::PERIODIC, md::env::BoundaryNormal::BACK);
            }

            types = simulation.get()->Boundary().typeBOTTOM();
            if (types == "OUTFLOW") {
                boundary.set_boundary_rule(md::env::BoundaryRule::OUTFLOW, md::env::BoundaryNormal::BOTTOM);

            } else if (types == "VELOCITY_REFLECTION") {
                boundary.set_boundary_rule(md::env::BoundaryRule::VELOCITY_REFLECTION, md::env::BoundaryNormal::BOTTOM);

            } else if (types == "REPULSIVE_FORCE") {
                boundary.set_boundary_rule(md::env::BoundaryRule::REPULSIVE_FORCE, md::env::BoundaryNormal::BOTTOM);

            } else {
                boundary.set_boundary_rule(md::env::BoundaryRule::PERIODIC, md::env::BoundaryNormal::BOTTOM);
            }

            types = simulation.get()->Boundary().typeLEFT();

            if (types == "OUTFLOW") {
                boundary.set_boundary_rule(md::env::BoundaryRule::OUTFLOW, md::env::BoundaryNormal::LEFT);

            } else if (types == "VELOCITY_REFLECTION") {
                boundary.set_boundary_rule(md::env::BoundaryRule::VELOCITY_REFLECTION, md::env::BoundaryNormal::LEFT);

            } else if (types == "REPULSIVE_FORCE") {
                boundary.set_boundary_rule(md::env::BoundaryRule::REPULSIVE_FORCE, md::env::BoundaryNormal::LEFT);

            } else {
                boundary.set_boundary_rule(md::env::BoundaryRule::PERIODIC, md::env::BoundaryNormal::LEFT);
            }

            types = simulation.get()->Boundary().typeRIGHT();
            if (types == "OUTFLOW") {
                boundary.set_boundary_rule(md::env::BoundaryRule::OUTFLOW, md::env::BoundaryNormal::RIGHT);

            } else if (types == "VELOCITY_REFLECTION") {
                boundary.set_boundary_rule(md::env::BoundaryRule::VELOCITY_REFLECTION, md::env::BoundaryNormal::RIGHT);

            } else if (types == "REPULSIVE_FORCE") {
                boundary.set_boundary_rule(md::env::BoundaryRule::REPULSIVE_FORCE, md::env::BoundaryNormal::RIGHT);

            } else {
                boundary.set_boundary_rule(md::env::BoundaryRule::PERIODIC, md::env::BoundaryNormal::RIGHT);
            }

            types = simulation.get()->Boundary().typeTOP();

            if (types == "OUTFLOW") {
                boundary.set_boundary_rule(md::env::BoundaryRule::OUTFLOW, md::env::BoundaryNormal::TOP);

            } else if (types == "VELOCITY_REFLECTION") {
                boundary.set_boundary_rule(md::env::BoundaryRule::VELOCITY_REFLECTION, md::env::BoundaryNormal::TOP);

            } else if (types == "REPULSIVE_FORCE") {
                boundary.set_boundary_rule(md::env::BoundaryRule::REPULSIVE_FORCE, md::env::BoundaryNormal::TOP);

            } else {
                boundary.set_boundary_rule(md::env::BoundaryRule::PERIODIC, md::env::BoundaryNormal::TOP);
            }

            if (simulation.get()->Forces().Force().get().type() == "lennardJones") {
                args.env.set_force(env::LennardJones(simulation.get()->Forces().Force().get().arg1().get(),
                                                     simulation.get()->Forces().Force().get().arg2().get()));
                args.force = "lennardJones";
                boundary.set_boundary_force(
                    env::Boundary::LennardJonesForce(simulation.get()->Forces().Force().get().arg1().get(),
                                                     simulation.get()->Forces().Force().get().arg2().get()));

            } else if (simulation.get()->Forces().Force().get().type() == "inverseSquare") {
                args.env.set_force(env::InverseSquare(simulation.get()->Forces().Force().get().arg1().get()));
                args.force = "inverseSquare";
                boundary.set_boundary_force(
                    env::Boundary::InverseDistanceForce(simulation.get()->parameters().cutoff_radius(),
                                                        simulation.get()->Forces().Force().get().arg1().get()));
            } else if (simulation.get()->Forces().Force().get().type() == "HooksLaw") {
                args.env.set_force(env::HookesLaw(simulation.get()->Forces().Force().get().arg1().get(),
                                                  simulation.get()->Forces().Force().get().arg2().get()));
                args.force = "HooksLaw";
            } else {
                args.force = "no force applied";
            }

            args.env.set_boundary(boundary);

            args.cutoff_radius = simulation.get()->parameters().cutoff_radius();

            args.write_freq = simulation.get()->output().writeFrequency();
            file.close();

        } catch (const xml_schema::exception& e) {
            file.close();
            RETURN_PARSE_ERROR(fmt::format("XML parsing error: {}", e.what()));
        }

        if (args.benchmark) {
            args.output_format = io::OutputFormat::XYZ;
            return OK;
        }

        if (parameters[2] != "XYZ" && parameters[2] != "VTK") {
            RETURN_PARSE_ERROR(fmt::format("Error: invalid file output format: {}", parameters[2]));
        }
        args.output_format = parameters[2] == "XYZ" ? io::OutputFormat::XYZ : io::OutputFormat::VTK;

        SPDLOG_INFO("Arguments parsed successfully");
        return OK;
    }
}  // namespace md::parse
