#include "XMLFileReader.h"

#include <algorithm>
#include <fstream>
#include <string>

#include "env/Environment.h"
#include "io/input/xml/molSimSchema.hxx"
#include "io/Logger/Logger.h"
#include "utils/Parse.h"

#define ERROR_AND_EXIT(err_msg)                                                      \
        file.close();                                                                \
        SPDLOG_ERROR(err_msg);                                                       \
        exit(-1);                                                                    \

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
            args.output_baseName = static_cast<std::string>(simulation->output().base_name());
            args.write_freq = simulation->output().write_frequency();
            args.duration = simulation->parameters().end_t();
            args.dt = simulation->parameters().delta_t();
            args.cutoff_radius = simulation->parameters().cutoff_radius();

            std::string strategy = simulation->parameters().parallel_strategy();
            if (strategy != "NONE" && strategy != "CELL_LOCK" && strategy != "SPATIAL_DECOMPOSITION") {
                ERROR_AND_EXIT(fmt::format("Invalid parallelization strategy: {}", strategy));
            }
            if (strategy == "CELL_LOCK") args.parallel_strategy = 1;
            else if (strategy == "SPATIAL_DECOMPOSITION") args.parallel_strategy = 2;
            else args.parallel_strategy = 0;


            /// -----------------------------------------
            ///  Parse particle information
            /// -----------------------------------------
            for (const auto& particle : simulation->particles()) {
                args.env.add_particle({particle.origin()[0], particle.origin()[1], particle.origin()[2]},
                                      {particle.velocity()[0], particle.velocity()[1], particle.velocity()[2]},
                                      particle.mass(), particle.type());

                SPDLOG_DEBUG(fmt::format(
                        "Parsed Particle:\n"
                        "       Origin:           [{}, {}, {}]\n"
                        "       Initial Velocity: [{}, {}, {}]\n"
                        "       Mass:             {}\n"
                        "       Type:             {}",
                        particle.origin()[0], particle.origin()[1], particle.origin()[2],
                        particle.velocity()[0], particle.velocity()[1], particle.velocity()[2],
                        particle.mass(), particle.type()));
            }


            /// -----------------------------------------
            ///  Parse cuboid information
            /// -----------------------------------------
            for (const auto& cuboid : simulation->cuboids()) {
                const auto dim = static_cast<int>(cuboid.dimension());
                if (dim != 2 && dim != 3 && dim != -1) {
                    ERROR_AND_EXIT(fmt::format("Invalid dimension parameter {}", dim));
                }
                const auto dimension = static_cast<Dimension>(dim);
                if (cuboid.state() != "ALIVE" && cuboid.state() != "STATIONARY") {
                    ERROR_AND_EXIT(fmt::format("Invalid particle state {}", cuboid.state()));
                }
                auto state = cuboid.state() == "ALIVE" ? Particle::ALIVE : Particle::STATIONARY;

                args.env.add_cuboid({cuboid.origin()[0], cuboid.origin()[1], cuboid.origin()[2]},
                                    {cuboid.velocity()[0], cuboid.velocity()[1], cuboid.velocity()[2]},
                                    {cuboid.numPart()[0], cuboid.numPart()[1], cuboid.numPart()[2]},
                                    cuboid.width(), cuboid.mass(), cuboid.thermal_v(), cuboid.type(), dimension, state);

                SPDLOG_DEBUG(fmt::format(
                        "Parsed Cuboid:\n"
                        "       Origin:              [{}, {}, {}]\n"
                        "       Initial Velocity:    [{}, {}, {}]\n"
                        "       Number of Particles: [{}, {}, {}]\n"
                        "       Width:               {}\n"
                        "       Mass:                {}\n"
                        "       Thermal Velocity:    {}\n"
                        "       Type:                {}\n"
                        "       Dimension:           {}\n"
                        "       State:               {}",
                        cuboid.origin()[0], cuboid.origin()[1], cuboid.origin()[2],
                        cuboid.velocity()[0], cuboid.velocity()[1], cuboid.velocity()[2],
                        cuboid.numPart()[0], cuboid.numPart()[1], cuboid.numPart()[2],
                        cuboid.width(), cuboid.mass(), cuboid.thermal_v(), cuboid.type(), dim,
                        cuboid.state() == "ALIVE" ? "ALIVE" : "STATIONARY"));
            }


            /// -----------------------------------------
            ///  Parse sphere information
            /// -----------------------------------------
            for (const auto& sphere : simulation->spheres()) {
                const auto dim = static_cast<int>(sphere.dimension());
                if (dim != 2 && dim != 3) {
                    ERROR_AND_EXIT(fmt::format("Invalid dimension parameter {}", dim));
                }
                const auto dimension = static_cast<Dimension>(dim);
                if (sphere.state() != "ALIVE" && sphere.state() != "STATIONARY") {
                    ERROR_AND_EXIT(fmt::format("Invalid particle state {}", sphere.state()));
                }
                auto state = sphere.state() == "ALIVE" ? Particle::ALIVE : Particle::STATIONARY;

                args.env.add_sphere({sphere.origin()[0], sphere.origin()[1], sphere.origin()[2]},
                                    {sphere.velocity()[0], sphere.velocity()[1], sphere.velocity()[2]},
                                    sphere.radius(), sphere.width(), sphere.mass(), sphere.thermal_v(),
                                    sphere.type(), dimension, state);

                SPDLOG_DEBUG(fmt::format(
                        "Parsed Sphere:\n"
                        "       Origin:              [{}, {}, {}]\n"
                        "       Initial Velocity:    [{}, {}, {}]\n"
                        "       Radius:              {}\n"
                        "       Width:               {}\n"
                        "       Mass:                {}\n"
                        "       Thermal Velocity:    {}\n"
                        "       Type:                {}\n"
                        "       Dimension:           {}\n"
                        "       State:               {}",
                        sphere.origin()[0], sphere.origin()[1], sphere.origin()[2],
                        sphere.velocity()[0], sphere.velocity()[1], sphere.velocity()[2],
                        sphere.radius(), sphere.width(), sphere.mass(), sphere.thermal_v(), sphere.type(), dim,
                        sphere.state() == "ALIVE" ? "ALIVE" : "STATIONARY"));
            }


            /// -----------------------------------------
            ///  Parse membrane information
            /// -----------------------------------------
            for (const auto& membrane : simulation->membranes()) {
                args.env.add_membrane({membrane.origin()[0], membrane.origin()[1], membrane.origin()[2]},
                                      {membrane.velocity()[0], membrane.velocity()[1], membrane.velocity()[2]},
                                      {membrane.numPart()[0], membrane.numPart()[1], membrane.numPart()[2]},
                                      membrane.width(), membrane.mass(), membrane.k(), args.cutoff_radius,
                                      membrane.type());

                SPDLOG_DEBUG(fmt::format(
                        "Parsed Membrane:\n"
                        "       Origin:              [{}, {}, {}]\n"
                        "       Initial Velocity:    [{}, {}, {}]\n"
                        "       Number of particles: [{}, {}, {}]\n"
                        "       Width:               {}\n"
                        "       Mass:                {}\n"
                        "       k:                   {}\n"
                        "       cutoff:              {}\n"
                        "       Type:                {}",
                        membrane.origin()[0], membrane.origin()[1], membrane.origin()[2],
                        membrane.velocity()[0], membrane.velocity()[1], membrane.velocity()[2],
                        membrane.numPart()[0], membrane.numPart()[1], membrane.numPart()[2],
                        membrane.width(), membrane.mass(), membrane.k(), args.cutoff_radius,
                        membrane.type()));
            }


            /// -----------------------------------------
            ///  Parse environment and boundary information
            /// -----------------------------------------
            auto boundary_xml = simulation->Boundary();

            auto extract_boundary_type = [&](const std::string& type) -> env::BoundaryRule {
                if (type == "OUTFLOW") return env::BoundaryRule::OUTFLOW;
                if (type == "VELOCITY_REFLECTION") return env::BoundaryRule::VELOCITY_REFLECTION;
                if (type == "REPULSIVE_FORCE") return env::BoundaryRule::REPULSIVE_FORCE;
                if (type == "PERIODIC") return env::BoundaryRule::PERIODIC;
                throw std::invalid_argument("Unknown boundary type: " + type);
            };

            env::Boundary boundary;
            boundary.extent = {boundary_xml.extent()[0], boundary_xml.extent()[1], boundary_xml.extent()[2]};
            SPDLOG_DEBUG(fmt::format("Parsed boundary extent: [{}, {}, {}]", boundary.extent[0], boundary.extent[1], boundary.extent[2]));

            boundary.origin = {boundary_xml.origin()[0], boundary_xml.origin()[1], boundary_xml.origin()[2]};
            SPDLOG_DEBUG(fmt::format("Parsed boundary origin: [{}, {}, {}]", boundary.origin[0], boundary.origin[1], boundary.origin[2]));

            boundary.set_boundary_rule(extract_boundary_type(boundary_xml.typeFRONT()), env::BoundaryNormal::FRONT);
            boundary.set_boundary_rule(extract_boundary_type(boundary_xml.typeBACK()), env::BoundaryNormal::BACK);
            boundary.set_boundary_rule(extract_boundary_type(boundary_xml.typeRIGHT()), env::BoundaryNormal::RIGHT);
            boundary.set_boundary_rule(extract_boundary_type(boundary_xml.typeTOP()), env::BoundaryNormal::TOP);
            boundary.set_boundary_rule(extract_boundary_type(boundary_xml.typeLEFT()), env::BoundaryNormal::LEFT);
            boundary.set_boundary_rule(extract_boundary_type(boundary_xml.typeBOTTOM()), env::BoundaryNormal::BOTTOM);

            SPDLOG_DEBUG(fmt::format("Parsed boundary rules - FRONT: {}, BACK: {}, RIGHT: {}, LEFT: {}, TOP: {}, BOTTOM: {}",
                         boundary_xml.typeFRONT(), boundary_xml.typeBACK(), boundary_xml.typeRIGHT(),
                         boundary_xml.typeLEFT(), boundary_xml.typeTOP(), boundary_xml.typeBOTTOM()));

            if (boundary_xml.force_type()) {
                if (boundary_xml.force_type().get() == "lennardJones") {
                    boundary.set_boundary_force(env::Boundary::LennardJonesForce(
                            boundary_xml.force_arg1().get(), boundary_xml.force_arg2().get()));

                    SPDLOG_DEBUG(fmt::format("Parsed boundary force: Lennard Jones with epsilon = {}, sigma = {}",
                                             boundary_xml.force_arg1().get(), boundary_xml.force_arg2().get()));
                }
                else if (boundary_xml.force_type().get() == "inverseSquare") {
                    boundary.set_boundary_force(env::Boundary::InverseDistanceForce(
                            args.cutoff_radius, boundary_xml.force_arg1().get()));

                    SPDLOG_DEBUG(fmt::format("Parsed boundary force: Inverse Square with cutoff = {}, pre factor = {}",
                                             args.cutoff_radius, boundary_xml.force_arg1().get()));
                }
                else {
                    ERROR_AND_EXIT("Parsing boundary force failed");
                }
            }

            args.env.set_grid_constant(boundary_xml.grid_constant());
            SPDLOG_DEBUG(fmt::format("Parsed grid constant: {}", boundary_xml.grid_constant()));


            /// -----------------------------------------
            ///  Parse force information
            /// -----------------------------------------
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
                    ERROR_AND_EXIT(fmt::format("Invalid force type: {}", force.type()));
                }
            }


            /// -----------------------------------------
            ///  Parse constant force information
            /// -----------------------------------------
            for (const auto& constant_force : simulation->ConstantForces().ConstantForce()) {
                if (constant_force.type() == "gravity") {
                    env::ConstantForce gravity = env::Gravity(constant_force.strength(),
                            {constant_force.direction()[0],constant_force.direction()[1], constant_force.direction()[2]});
                    args.external_forces.push_back(gravity);

                    SPDLOG_DEBUG("Parsed gravity force: \n"
                                 "       Direction:  [{}, {}, {}]\n"
                                 "       Strength:   {}",
                                 constant_force.direction()[0],constant_force.direction()[1],
                                 constant_force.direction()[2], constant_force.strength());
                }
                else if (constant_force.type() == "pullForce") {
                    env::ConstantForce pull_force (
                            {constant_force.direction()[0],constant_force.direction()[1], constant_force.direction()[2]},
                            constant_force.strength(),
                            env::MarkBox({constant_force.MarkBoxVec1().get()[0], constant_force.MarkBoxVec1().get()[1], constant_force.MarkBoxVec1().get()[2]},
                                         {constant_force.MarkBoxVec2().get()[0], constant_force.MarkBoxVec2().get()[1], constant_force.MarkBoxVec2().get()[2]}),
                            constant_force.start_t(), constant_force.end_t(), constant_force.const_acc());

                    args.external_forces.push_back(pull_force);
                    SPDLOG_DEBUG(fmt::format("Parsed pull force: \n"
                                 "       Direction:  [{}, {}, {}]\n"
                                 "       Strength:   {}\n"
                                 "       Markbox:    bottom left corner [{}, {}, {}] and top right corner [{}, {}, {}]\n"
                                 "       Start time: {}\n"
                                 "       End time:   {}\n"
                                 "       Constant acceleration: {}",
                                 constant_force.direction()[0],constant_force.direction()[1], constant_force.direction()[2], constant_force.strength(),
                                 constant_force.MarkBoxVec1().get()[0], constant_force.MarkBoxVec1().get()[1], constant_force.MarkBoxVec1().get()[2],
                                 constant_force.MarkBoxVec2().get()[0], constant_force.MarkBoxVec2().get()[1], constant_force.MarkBoxVec2().get()[2],
                                 constant_force.start_t(), constant_force.end_t(), constant_force.const_acc() ? "true" : "false"));
                }
                else {
                    ERROR_AND_EXIT(fmt::format("Invalid Constant Force type: {}", constant_force.type()));
                }
            }

            args.env.set_boundary(boundary);
            args.env.build();


            /// -----------------------------------------
            ///  Parse thermostats information
            /// -----------------------------------------
            if (simulation->Thermostat()) {
                args.temp_adj_freq = simulation->Thermostat()->n_thermostats();
                double delta_T =
                        simulation->Thermostat()->temp_dT().get() == -1 ? std::numeric_limits<double>::infinity() :
                        simulation->Thermostat()->temp_dT().get();
                args.thermostat.init(simulation->Thermostat()->init_T(),
                                     simulation->Thermostat()->target_T().get(), delta_T);
                SPDLOG_DEBUG(fmt::format(
                        "Parsed thermostat - Initial temperature: {}, n_thermostat: {}, Target temperature: {}, delta T: {}",
                        simulation->Thermostat()->init_T(), args.temp_adj_freq, simulation->Thermostat()->target_T().get(), delta_T));
                args.thermostat.set_initial_temperature(args.env);
            }

            file.close();
        }
        catch (const xml_schema::exception& e) {
            ERROR_AND_EXIT(fmt::format("XML parsing error: {}", e.what()));
        }

        SPDLOG_INFO("File successfully read: {}", file_name);
    }
}  // namespace md::io