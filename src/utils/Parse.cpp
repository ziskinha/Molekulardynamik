#include "Parse.h"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include "io/input/xml/molSimSchema.hxx"
#include <fstream>


#define RETURN_PARSE_ERROR(err_msg)                                                \
    SPDLOG_ERROR(err_msg);                                                         \
    SPDLOG_ERROR("For help, run the program with ./MolSim -h or ./MolSim --help"); \
    return ERROR;

#define PARSE_NUMBER(index, target, type, name)                                                                        \
    try {                                                                                                              \
        target = type(parameters[index]);                                                                              \
    } catch (const std::invalid_argument& e) {                                                                         \
        RETURN_PARSE_ERROR(fmt::format("Error while reading {} argument! Caught an invalid_argument exception", name)) \
    }

namespace md::parse {
    void displayHelp() {
        SPDLOG_INFO(
            "Displaying help information.\n"
            "Usage:\n"
            "  ./MolSim <xml_file> <output_format>\n"
            "  ./MolSim -h | --help\n"
            "  ./MolSim -f\n"
            "  ./MolSim -b\n"
            "  ./MolSim -v\n\n"
            "Arguments:\n"
            "  xml_file         file with input parameters important for the simulation. For more information about the structure see ../io/molSimSchema.xsd\n"
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
             std::cerr << "Error: Unable to open xml file: " << arguments[2] << '\n';
             return EXIT;
        }

        auto simulation = simulation_(file,xml_schema::flags::dont_validate); 
        for (const auto& particle : simulation.get()->particles())
        {
            args.env.add_particle({particle.x(),particle.y(),particle.z()}, {particle.vel1(),particle.vel2(),particle.vel3()},particle.mass(),0);
        }
        
        for (const auto& cuboid : simulation.get()->cuboids()) {
            args.env.add_cuboid({cuboid.x(),cuboid.y(),cuboid.z()}, {cuboid.vel1(),cuboid.vel2(),cuboid.vel3()}, {40,8,1}, cuboid.thermal_v(), cuboid.width(), cuboid.mass(), cuboid.dimension(), 0);
            }
            
        args.output_baseName= simulation.get()->output().baseName();
        args.duration = simulation.get()->parameters().tEnd();
        args.dt = simulation.get()->parameters().deltaT();

      
        

        env::Boundary boundary;
        boundary.extent = {simulation.get()->Boundary().EXTENT_WIDTH().get(), simulation.get()->Boundary().EXTENT_HEIGHT().get(), simulation.get()->Boundary().EXTENT_DEPTH().get()};
        boundary.origin = {simulation.get()->Boundary().CENTER_BOUNDARY_ORIGINX().get(), simulation.get()->Boundary().CENTER_BOUNDARY_ORIGINY().get(), simulation.get()->Boundary().CENTER_BOUNDARY_ORIGINZ().get()};
        if(simulation.get()->Boundary().typeFRONT()=="OUTFLOW"){
        boundary.set_boundary_rule(md::env::BoundaryRule::OUTFLOW);

        }else if(simulation.get()->Boundary().typeFRONT()=="VELOCITY_REFLECTION"){
        boundary.set_boundary_rule(md::env::BoundaryRule::VELOCITY_REFLECTION);

        }else if (simulation.get()->Boundary().typeFRONT()=="REPULSIVE_FORCE"){
        boundary.set_boundary_rule(md::env::BoundaryRule::REPULSIVE_FORCE);

        }else{
        boundary.set_boundary_rule(md::env::BoundaryRule::PERIODIC);
        }
       
          if(simulation.get()->Forces().Force().get().type()=="lennardJones"){
            args.env.set_force(env::LennardJones(simulation.get()->Forces().Force().get().arg1().get(),simulation.get()->Forces().Force().get().arg2().get()));
            args.force="lennardJones";
            boundary.set_boundary_force(env::Boundary::LennardJonesForce(simulation.get()->Forces().Force().get().arg1().get(),simulation.get()->Forces().Force().get().arg2().get()));

        }
        else if(simulation.get()->Forces().Force().get().type()=="inverseSquare"){
        args.env.set_force(env::InverseSquare(simulation.get()->Forces().Force().get().arg1().get()));
        args.force="inverseSquare";
        boundary.set_boundary_force(env::Boundary::InverseDistanceForce(simulation.get()->parameters().cutoff_radius(),simulation.get()->Forces().Force().get().arg1().get()));
        }
        else if(simulation.get()->Forces().Force().get().type()=="HooksLaw"){
        args.env.set_force(env::HookesLaw(simulation.get()->Forces().Force().get().arg1().get(), simulation.get()->Forces().Force().get().arg2().get()));
        args.force="HooksLaw";
        }
        else{
            args.force="no force applied";
        }

        args.env.set_boundary(boundary);

        args.cutoff_radius= simulation.get()->parameters().cutoff_radius();

        args.write_freq= simulation.get()->output().writeFrequency();
        file.close();
       
            }
            catch (const xml_schema::exception& e) {
        std::cerr << "XML parsing error: " << e.what() << std::endl;
        file.close();
        return EXIT;
    }

        if (args.benchmark) {
            args.output_format=io::OutputFormat::XYZ;
            return OK;
        }

        if (parameters[2] != "XYZ" && parameters[2] != "VTK") {
            RETURN_PARSE_ERROR(fmt::format("Error: invalid file output format: {}", args.file));
        }
        args.output_format = parameters[2] == "XYZ" ? io::OutputFormat::XYZ : io::OutputFormat::VTK;

        SPDLOG_INFO("Arguments parsed successfully");
        return OK;
    }
}  // namespace md::parse
