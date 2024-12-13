#include <fstream>
#include <algorithm>
#include <string>

#include "XMLFileReader.h"
#include "env/Environment.h"
#include "utils/Parse.h"
#include "io/Input/xml/molSimSchema.hxx"
#include "io/Logger/Logger.h"

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
            for (const auto& particle : simulation->particles()) {
                args.env.add_particle({particle.x(), particle.y(), particle.z()},
                                      {particle.vel1(), particle.vel2(), particle.vel3()}, particle.mass(), 0);
            }

            for (const auto& cuboid : simulation->cuboids()) {
                args.env.add_cuboid({cuboid.x(), cuboid.y(), cuboid.z()}, {cuboid.vel1(), cuboid.vel2(), cuboid.vel3()},
                                    {cuboid.numPartX(), cuboid.numPartY(),cuboid.numPartZ()}, cuboid.thermal_v(), cuboid.width(), cuboid.mass(), cuboid.dimension(),
                                    0);
            }

            for (const auto& sphere : simulation->spheres()) {
                args.env.add_sphere({sphere.x(), sphere.y(), sphere.z()}, {sphere.vel1(), sphere.vel2(), sphere.vel3()},
                                    sphere.thermal_v(), sphere.radius(), sphere.width(), sphere.mass(),
                                    sphere.dimension(), 0);
            }
            args.temp_adj_freq= simulation->Thermostat().n_thermostats().get();
            args.init_T=simulation->Thermostat().init_T().get();
            args.target_T=simulation->Thermostat().target_T().get();
            args.temp_dT = simulation->Thermostat().temp_dT().get();

            args.output_baseName = static_cast<std::string>(simulation->output().baseName());
            
            args.duration = simulation->parameters().tEnd();
            
            args.dt = simulation->parameters().deltaT();
            
            args.gravitational_force= simulation->parameters().gravitational_force();
            
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

            //todo edit
           
            args.env.set_grid_constant(0.5);
            args.env.set_boundary(boundary);
           
            args.cutoff_radius = simulation->parameters().cutoff_radius();
            args.write_freq = simulation->output().writeFrequency();
            args.env.build();
            file.close();

        } catch (const xml_schema::exception& e) {
            file.close();
            SPDLOG_ERROR(fmt::format("XML parsing error: {}", e.what()));
            exit(-1);
        }

        SPDLOG_INFO("File read successfully: {}", file_name);
    }
} // namespace md::io