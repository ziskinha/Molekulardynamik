#include "XMLFileReader.h"

#include <algorithm>
#include <fstream>
#include <string>

#include "env/Environment.h"
#include "io/input/xml/molSimSchema.hxx"
#include "io/Logger/Logger.h"
#include "utils/Parse.h"

namespace md::io {
    using namespace env;
    using namespace parse;

    void read_file_xml(const std::string& file_name, ProgramArguments& args) {
        std::ifstream file(file_name);

        try {
            if (!file.is_open()) {
                SPDLOG_ERROR("Unable to open xml file: {}", file_name);
                exit(-1);
            }

            SPDLOG_INFO("Start reading file {}", file_name);

            auto simulation = simulation_(file, xml_schema::flags::dont_validate);

            /// -----------------------------------------
            ///  Parse general information
            /// -----------------------------------------
            args.output_baseName = static_cast<std::string>(simulation->output().baseName());
            args.duration = simulation->parameters().tEnd();
            args.dt = simulation->parameters().deltaT();
            args.write_freq = simulation->output().writeFrequency();

            /// -----------------------------------------
            ///  Parse particle information
            /// -----------------------------------------
            for (const auto& particle : simulation->particles()) {
                args.env.add_particle({particle.x(), particle.y(), particle.z()},
                                      {particle.vel1(), particle.vel2(), particle.vel3()}, particle.mass(),
                                      particle.type());
                SPDLOG_DEBUG(fmt::format(
                        "Parsed Particle:\n"
                        "       Origin:           [{}, {}, {}]\n"
                        "       Initial Velocity: [{}, {}, {}]\n"
                        "       Mass:             {}\n"
                        "       Type:             {}",
                        particle.x(), particle.y(), particle.z(), particle.vel1(), particle.vel2(), particle.vel3(),
                        particle.mass(), particle.type()));
            }

            /// -----------------------------------------
            ///  Parse cuboid information
            /// -----------------------------------------
            for (const auto& cuboid : simulation->cuboids()) {
                const auto dim = static_cast<uint32_t>(cuboid.dimension());
                if (dim != 2 && dim != 3 && dim != -1) {
                    SPDLOG_ERROR("Invalid dimension parameter {}", dim);
                }
                const auto dimension = static_cast<Dimension>(dim);

                args.env.add_cuboid({cuboid.x(), cuboid.y(), cuboid.z()}, {cuboid.vel1(), cuboid.vel2(), cuboid.vel3()},
                                    {cuboid.numPartX(), cuboid.numPartY(), cuboid.numPartZ()},
                                    cuboid.width(), cuboid.mass(), cuboid.thermal_v(), cuboid.type(), dimension, Particle::ALIVE); // TODO read particle type from XML
                SPDLOG_DEBUG(fmt::format(
                        "Parsed Cuboid:\n"
                        "       Origin:              [{}, {}, {}]\n"
                        "       Initial Velocity:    [{}, {}, {}]\n"
                        "       Number of Particles: [{}, {}, {}]\n"
                        "       Width:               {}\n"
                        "       Mass:                {}\n"
                        "       Thermal Velocity:    {}\n"
                        "       Type:                {}",
                        cuboid.x(), cuboid.y(), cuboid.z(), cuboid.vel1(), cuboid.vel2(), cuboid.vel3(),
                        cuboid.numPartX(), cuboid.numPartY(), cuboid.numPartZ(), cuboid.width(), cuboid.mass(),
                        cuboid.thermal_v(), cuboid.type()));
            }

            /// -----------------------------------------
            ///  Parse sphere information
            /// -----------------------------------------
            for (const auto& sphere : simulation->spheres()) {
                const auto dim = static_cast<uint32_t>(sphere.dimension());
                if (dim != 2 && dim != 3) {
                    SPDLOG_ERROR("Invalid dimension parameter {}", dim);
                }
                const auto dimension = static_cast<Dimension>(dim);
                args.env.add_sphere({sphere.x(), sphere.y(), sphere.z()}, {sphere.vel1(), sphere.vel2(), sphere.vel3()},
                    sphere.radius(), sphere.width(), sphere.mass(), sphere.thermal_v(),
                                    sphere.type(), dimension, Particle::ALIVE);  // TODO read particle type from XML
                SPDLOG_DEBUG(fmt::format(
                        "Parsed Sphere:\n"
                        "       Origin:              [{}, {}, {}]\n"
                        "       Initial Velocity:    [{}, {}, {}]\n"
                        "       Radius:              {}\n"
                        "       Width:               {}\n"
                        "       Mass:                {}\n"
                        "       Thermal Velocity:    {}\n"
                        "       Type:                {}",
                        sphere.x(), sphere.y(), sphere.z(), sphere.vel1(), sphere.vel2(), sphere.vel3(),
                        sphere.radius(), sphere.width(), sphere.mass(), sphere.thermal_v(), sphere.type()));
            }

            /// -----------------------------------------
            ///  Parse environment and boundary information
            /// -----------------------------------------
            args.env.set_gravity_constant(simulation->parameters().gravitational_force());
            SPDLOG_DEBUG(fmt::format("Parsed gravity constant: {}", simulation->parameters().gravitational_force()));

            std::string boundary_type;

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
            SPDLOG_DEBUG(fmt::format("Parsed boundary extent: [{}, {}, {}]", boundary.extent[0], boundary.extent[1], boundary.extent[2]));

            boundary.origin = {boundary_xml.CENTER_BOUNDARY_ORIGINX().get(),
                               boundary_xml.CENTER_BOUNDARY_ORIGINY().get(),
                               boundary_xml.CENTER_BOUNDARY_ORIGINZ().get()};
            SPDLOG_DEBUG(fmt::format("Parsed boundary origin: [{}, {}, {}]", boundary.origin[0], boundary.origin[1], boundary.origin[2]));

            boundary.set_boundary_rule(extract_boundary_type(boundary_xml.typeFRONT()), env::BoundaryNormal::FRONT);
            boundary.set_boundary_rule(extract_boundary_type(boundary_xml.typeBACK()), env::BoundaryNormal::BACK);
            boundary.set_boundary_rule(extract_boundary_type(boundary_xml.typeRIGHT()), env::BoundaryNormal::RIGHT);
            boundary.set_boundary_rule(extract_boundary_type(boundary_xml.typeTOP()), env::BoundaryNormal::TOP);
            boundary.set_boundary_rule(extract_boundary_type(boundary_xml.typeLEFT()), env::BoundaryNormal::LEFT);
            boundary.set_boundary_rule(extract_boundary_type(boundary_xml.typeBOTTOM()), env::BoundaryNormal::BOTTOM);

            SPDLOG_DEBUG(fmt::format("Parsed boundary rules - FRONT: {}, BACK: {}, RIGHT: {}, TOP: {}, LEFT: {}, BOTTOM: {}",
                         boundary_xml.typeFRONT(), boundary_xml.typeBACK(), boundary_xml.typeRIGHT(),
                         boundary_xml.typeTOP(), boundary_xml.typeLEFT(), boundary_xml.typeBOTTOM()));

            if (simulation->Boundary().Force_type().get() == "lennardJones") {
                boundary.set_boundary_force(env::Boundary::LennardJonesForce(
                    simulation->Boundary().force_arg1().get(), simulation->Boundary().force_arg2().get()));
                SPDLOG_DEBUG(fmt::format("Parsed boundary force: Lennard Jones with epsilon = {}, sigma = {}",
                             simulation->Boundary().force_arg1().get(), simulation->Boundary().force_arg2().get()));
            }
            else if (simulation->Boundary().Force_type().get() == "inverseSquare") {
                boundary.set_boundary_force(env::Boundary::InverseDistanceForce(
                    simulation->parameters().cutoff_radius(), simulation->Boundary().force_arg1().get()));
                SPDLOG_DEBUG(fmt::format("Parsed boundary force: Inverse Square with cutoff = {}, pre factor = {}",
                             simulation.get()->parameters().cutoff_radius(), simulation->Boundary().force_arg1().get()));
            }
            else {
                file.close();
                SPDLOG_ERROR("Error while parsing boundary force");
                exit(-1);
            }

            /// -----------------------------------------
            ///  Parse force information
            /// -----------------------------------------
            args.cutoff_radius = simulation->parameters().cutoff_radius();

            for (const auto& force : simulation->Forces().Force()) {
                if (force.type() == "lennardJones") {
                    args.env.set_force(env::LennardJones(force.arg1().get(), force.arg2().get(), args.cutoff_radius),
                                       force.partType());
                    SPDLOG_DEBUG(fmt::format("Parsed force for particle type {}: Lennard Jones with epsilon = {}, sigma = {}, "
                                 "cutoff radius = {}",
                                 force.partType(), force.arg1().get(), force.arg2().get(), args.cutoff_radius));
                }
                else if (force.type() == "inverseSquare") {
                    args.env.set_force(env::InverseSquare(force.arg1().get(), args.cutoff_radius), force.partType());
                    SPDLOG_DEBUG(fmt::format("Parsed force for particle type {}: Inverse Square with pre factor = {}, cutoff radius = {}",
                                 force.partType(), force.arg1().get(), args.cutoff_radius));
                }
                else {
                    file.close();
                    SPDLOG_ERROR("Error while parsing forces");
                    exit(-1);
                }
            }

            args.env.set_grid_constant(simulation->GridConstant());
            SPDLOG_DEBUG(fmt::format("Parsed grid constant: {}", simulation->GridConstant())),

            args.env.set_boundary(boundary);
            args.env.build();

            /// -----------------------------------------
            ///  Parse thermostats information
            /// -----------------------------------------
            if (simulation->Thermostat()) {
                args.temp_adj_freq = simulation->Thermostat()->n_thermostats().get();
                double delta_T =
                        simulation->Thermostat()->temp_dT().get() == -1 ? std::numeric_limits<double>::infinity() :
                        simulation->Thermostat()->temp_dT().get();
                args.thermostat.init(simulation->Thermostat()->init_T().get(),
                                     simulation->Thermostat()->target_T().get(), delta_T);
                SPDLOG_DEBUG(fmt::format(
                        "Parsed thermostat - Initial temperature: {}, n_thermostat: {}, Target temperature: {}, delta T: {}",
                        simulation->Thermostat()->init_T().get(), args.temp_adj_freq, simulation->Thermostat()->target_T().get(), delta_T));
                args.thermostat.set_initial_temperature(args.env);
            }

            file.close();
        }
        catch (const xml_schema::exception& e) {
            file.close();
            SPDLOG_ERROR(fmt::format("XML parsing error: {}", e.what()));
            exit(-1);
        }

        SPDLOG_INFO("File successfully read: {}", file_name);
    }
}  // namespace md::io