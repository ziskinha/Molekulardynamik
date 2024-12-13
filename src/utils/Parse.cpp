#include <algorithm>
#include <string>
#include <vector>

#include "Parse.h"
#include "io/IOStrategy.h"

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
            "  xml_file         file with Input parameters important for the simulation. For more information about "
            "the structure see ../io/molSimSchema.xsd\n"
            "  output_format    Output format: either 'XYZ' or 'VTK'.\n\n"
            "Flags:\n"
            "  -h, --help       Show this help message and exit.\n"
            "  -f               Delete all contents of the output folder before writing.\n"
            "  -b               Benchmark the simulation (output_format and output_folder optional).");
    }

    ParseStatus parse_args(int argc, char** argv, io::ProgramArguments& args) {
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
        if ((flag_exists("-b") && parameters.empty()) || parameters.size() < 2) {
            RETURN_PARSE_ERROR(
                fmt::format("Error: Not enough arguments provided. Received {} arguments.", parameters.size()));
        }

        io::read_file(arguments[1], args);

        // set options
        args.benchmark = flag_exists("-b");
        args.override = flag_exists("-f");

        std::ifstream file(arguments[1]);

        try {
            if (!file.is_open()) {
                RETURN_PARSE_ERROR(fmt::format("Unable to open xml file: {}", arguments[1]));
            }

            auto simulation = simulation_(file, xml_schema::flags::dont_validate);
            for (const auto& particle : simulation->particles()) {
                args.env.add_particle({particle.x(), particle.y(), particle.z()},
                                      {particle.vel1(), particle.vel2(), particle.vel3()}, particle.mass(), 0);
            }

            for (const auto& cuboid : simulation->cuboids()) {
                args.env.add_cuboid({cuboid.x(), cuboid.y(), cuboid.z()}, {cuboid.vel1(), cuboid.vel2(), cuboid.vel3()},
                                    {40, 8, 1}, cuboid.thermal_v(), cuboid.width(), cuboid.mass(), cuboid.dimension(),
                                    0);
            }

            for (const auto& sphere : simulation->spheres()) {
                args.env.add_sphere({sphere.x(), sphere.y(), sphere.z()}, {sphere.vel1(), sphere.vel2(), sphere.vel3()},
                                    sphere.thermal_v(), sphere.radius(), sphere.width(), sphere.mass(),
                                    sphere.dimension(), 0);
            }

           

            auto boundary_xml = simulation->Boundary();

            auto extract_boundary_type = [&](const std::string& type) -> env::BoundaryRule {
                if (type == "OUTFLOW") return env::BoundaryRule::OUTFLOW;
                if (type == "VELOCITY_REFLECTION") return env::BoundaryRule::VELOCITY_REFLECTION;
                if (type == "REPULSIVE_FORCE") return env::BoundaryRule::REPULSIVE_FORCE;
                if (type == "PERIODIC") return env::BoundaryRule::PERIODIC;
                throw std::invalid_argument("Unknown boundary type: " + type);
            };

            env::Boundary boundary;
            boundary.extent = {boundary_xml.EXTENT_WIDTH().get(),
                               boundary_xml.EXTENT_HEIGHT().get(),
                               boundary_xml.EXTENT_DEPTH().get()};
            boundary.origin = {boundary_xml.CENTER_BOUNDARY_ORIGINX().get(),
                               boundary_xml.CENTER_BOUNDARY_ORIGINY().get(),
                               boundary_xml.CENTER_BOUNDARY_ORIGINZ().get()};
                            
            boundary.set_boundary_rule(extract_boundary_type(boundary_xml.typeFRONT()), env::BoundaryNormal::FRONT);
            boundary.set_boundary_rule(extract_boundary_type(boundary_xml.typeBACK()), env::BoundaryNormal::BACK);
            boundary.set_boundary_rule(extract_boundary_type(boundary_xml.typeRIGHT()), env::BoundaryNormal::RIGHT);
            boundary.set_boundary_rule(extract_boundary_type(boundary_xml.typeTOP()), env::BoundaryNormal::TOP);
            boundary.set_boundary_rule(extract_boundary_type(boundary_xml.typeLEFT()), env::BoundaryNormal::LEFT);
            boundary.set_boundary_rule(extract_boundary_type(boundary_xml.typeBOTTOM()), env::BoundaryNormal::BOTTOM);

             if (simulation.get()->Forces().Force().get().type() == "lennardJones") {
                 args.env.set_force(env::LennardJones(simulation.get()->Forces().Force().get().arg1().get(),
                                                     simulation.get()->Forces().Force().get().arg2().get(),args.cutoff_radius),1);
                 args.force = "lennardJones";
                 boundary.set_boundary_force(
                     env::Boundary::LennardJonesForce(simulation.get()->Forces().Force().get().arg1().get(),
                                                      simulation.get()->Forces().Force().get().arg2().get()));
            
             } else if (simulation.get()->Forces().Force().get().type() == "inverseSquare") {
                 args.env.set_force(env::InverseSquare(simulation.get()->Forces().Force().get().arg1().get(),args.cutoff_radius),1);
                 args.force = "inverseSquare";
                 boundary.set_boundary_force(
                     env::Boundary::InverseDistanceForce(simulation.get()->parameters().cutoff_radius(),
                                                         simulation.get()->Forces().Force().get().arg1().get()));
             }  else {
                 args.force = "no force applied";
             }

            args.env.set_boundary(boundary);

            args.cutoff_radius = simulation->parameters().cutoff_radius();
            args.gravitational_force = simulation->parameters().gravitational_force();
            args.write_freq = simulation->output().writeFrequency();
           // args.env.set_grid_constant(2.5);
            args.env.set_gravity_constant(simulation->parameters().gravitational_force());
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
        io::log_arguments(args);
        return OK;
    }
}  // namespace md::parse
