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
                                    {40, 8, 1}, cuboid.thermal_v(), cuboid.width(), cuboid.mass(), cuboid.dimension(),
                                    0);
            }

            for (const auto& sphere : simulation->spheres()) {
                args.env.add_sphere({sphere.x(), sphere.y(), sphere.z()}, {sphere.vel1(), sphere.vel2(), sphere.vel3()},
                                    sphere.thermal_v(), sphere.radius(), sphere.width(), sphere.mass(),
                                    sphere.dimension(), 0);
            }

            args.output_baseName = static_cast<std::string>(simulation->output().baseName());
            args.duration = simulation->parameters().tEnd();
            args.dt = simulation->parameters().deltaT();
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

            // if (types == "OUTFLOW") {
            //     boundary.set_boundary_rule(md::env::BoundaryRule::OUTFLOW, md::env::BoundaryNormal::FRONT);
            //
            // } else if (types == "VELOCITY_REFLECTION") {
            //     boundary.set_boundary_rule(md::env::BoundaryRule::VELOCITY_REFLECTION, md::env::BoundaryNormal::FRONT);
            //
            // } else if (types == "REPULSIVE_FORCE") {
            //     boundary.set_boundary_rule(md::env::BoundaryRule::REPULSIVE_FORCE, md::env::BoundaryNormal::FRONT);
            //
            // } else {
            //     boundary.set_boundary_rule(md::env::BoundaryRule::PERIODIC, md::env::BoundaryNormal::FRONT);
            // }
            //
            // types = simulation.get()->Boundary().typeBACK();
            //
            // if (types == "OUTFLOW") {
            //     boundary.set_boundary_rule(md::env::BoundaryRule::OUTFLOW, md::env::BoundaryNormal::BACK);
            //
            // } else if (types == "VELOCITY_REFLECTION") {
            //     boundary.set_boundary_rule(md::env::BoundaryRule::VELOCITY_REFLECTION, md::env::BoundaryNormal::BACK);
            //
            // } else if (types == "REPULSIVE_FORCE") {
            //     boundary.set_boundary_rule(md::env::BoundaryRule::REPULSIVE_FORCE, md::env::BoundaryNormal::BACK);
            //
            // } else {
            //     boundary.set_boundary_rule(md::env::BoundaryRule::PERIODIC, md::env::BoundaryNormal::BACK);
            // }
            //
            // types = simulation.get()->Boundary().typeBOTTOM();
            // if (types == "OUTFLOW") {
            //     boundary.set_boundary_rule(md::env::BoundaryRule::OUTFLOW, md::env::BoundaryNormal::BOTTOM);
            //
            // } else if (types == "VELOCITY_REFLECTION") {
            //     boundary.set_boundary_rule(md::env::BoundaryRule::VELOCITY_REFLECTION, md::env::BoundaryNormal::BOTTOM);
            //
            // } else if (types == "REPULSIVE_FORCE") {
            //     boundary.set_boundary_rule(md::env::BoundaryRule::REPULSIVE_FORCE, md::env::BoundaryNormal::BOTTOM);
            //
            // } else {
            //     boundary.set_boundary_rule(md::env::BoundaryRule::PERIODIC, md::env::BoundaryNormal::BOTTOM);
            // }
            //
            // types = simulation.get()->Boundary().typeLEFT();
            //
            // if (types == "OUTFLOW") {
            //     boundary.set_boundary_rule(md::env::BoundaryRule::OUTFLOW, md::env::BoundaryNormal::LEFT);
            //
            // } else if (types == "VELOCITY_REFLECTION") {
            //     boundary.set_boundary_rule(md::env::BoundaryRule::VELOCITY_REFLECTION, md::env::BoundaryNormal::LEFT);
            //
            // } else if (types == "REPULSIVE_FORCE") {
            //     boundary.set_boundary_rule(md::env::BoundaryRule::REPULSIVE_FORCE, md::env::BoundaryNormal::LEFT);
            //
            // } else {
            //     boundary.set_boundary_rule(md::env::BoundaryRule::PERIODIC, md::env::BoundaryNormal::LEFT);
            // }
            //
            // types = simulation.get()->Boundary().typeRIGHT();
            // if (types == "OUTFLOW") {
            //     boundary.set_boundary_rule(md::env::BoundaryRule::OUTFLOW, md::env::BoundaryNormal::RIGHT);
            //
            // } else if (types == "VELOCITY_REFLECTION") {
            //     boundary.set_boundary_rule(md::env::BoundaryRule::VELOCITY_REFLECTION, md::env::BoundaryNormal::RIGHT);
            //
            // } else if (types == "REPULSIVE_FORCE") {
            //     boundary.set_boundary_rule(md::env::BoundaryRule::REPULSIVE_FORCE, md::env::BoundaryNormal::RIGHT);
            //
            // } else {
            //     boundary.set_boundary_rule(md::env::BoundaryRule::PERIODIC, md::env::BoundaryNormal::RIGHT);
            // }
            //
            // types = simulation.get()->Boundary().typeTOP();
            //
            // if (types == "OUTFLOW") {
            //     boundary.set_boundary_rule(md::env::BoundaryRule::OUTFLOW, md::env::BoundaryNormal::TOP);
            //
            // } else if (types == "VELOCITY_REFLECTION") {
            //     boundary.set_boundary_rule(md::env::BoundaryRule::VELOCITY_REFLECTION, md::env::BoundaryNormal::TOP);
            //
            // } else if (types == "REPULSIVE_FORCE") {
            //     boundary.set_boundary_rule(md::env::BoundaryRule::REPULSIVE_FORCE, md::env::BoundaryNormal::TOP);
            //
            // } else {
            //     boundary.set_boundary_rule(md::env::BoundaryRule::PERIODIC, md::env::BoundaryNormal::TOP);
            // }

            // TODO fix this
            // if (simulation.get()->Forces().Force().get().type() == "lennardJones") {
            //     args.env.set_force(env::LennardJones(simulation.get()->Forces().Force().get().arg1().get(),
            //                                          simulation.get()->Forces().Force().get().arg2().get()));
            //     args.force = "lennardJones";
            //     boundary.set_boundary_force(
            //         env::Boundary::LennardJonesForce(simulation.get()->Forces().Force().get().arg1().get(),
            //                                          simulation.get()->Forces().Force().get().arg2().get()));
            //
            // } else if (simulation.get()->Forces().Force().get().type() == "inverseSquare") {
            //     args.env.set_force(env::InverseSquare(simulation.get()->Forces().Force().get().arg1().get()));
            //     args.force = "inverseSquare";
            //     boundary.set_boundary_force(
            //         env::Boundary::InverseDistanceForce(simulation.get()->parameters().cutoff_radius(),
            //                                             simulation.get()->Forces().Force().get().arg1().get()));
            // } else if (simulation.get()->Forces().Force().get().type() == "HooksLaw") {
            //     args.env.set_force(env::HookesLaw(simulation.get()->Forces().Force().get().arg1().get(),
            //                                       simulation.get()->Forces().Force().get().arg2().get()));
            //     args.force = "HooksLaw";
            // } else {
            //     args.force = "no force applied";
            // }

            args.env.set_boundary(boundary);
            args.env.build();

            args.cutoff_radius = simulation->parameters().cutoff_radius();

            args.write_freq = simulation->output().writeFrequency();
            file.close();

        } catch (const xml_schema::exception& e) {
            file.close();
            SPDLOG_ERROR(fmt::format("XML parsing error: {}", e.what()));
            exit(-1);
        }

        SPDLOG_INFO("File read successfully: {}", file_name);
    }
} // namespace md::io