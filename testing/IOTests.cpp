#include <gtest/gtest.h>
#include<fstream>
#include<string>

#include "../src/env/Environment.h"
#include "../src/env/Force.h"
#include "../src/effects/ConstantForce.h"
#include "../src/io/IOStrategy.h"
#include "../src/io/input/txt/TXTFileReader.h"
#include "../src/io/input/xml/XMLFileReader.h"

using namespace md;

namespace md::env {
    class FriendClassForTests {
    public:
        /// -----------------------------------------
        /// Environment Class
        /// -----------------------------------------
        double get_grid_constant(Environment &env) { return env.grid_constant; }

        /// -----------------------------------------
        /// Thermostat Class
        /// -----------------------------------------
        double get_init_temp(Thermostat &thermostat) { return thermostat.init_temp; }
        double get_target_temp(Thermostat &thermostat) { return thermostat.target_temp; }
        double get_max_temp_change(Thermostat &thermostat) { return thermostat.max_temp_change;}

        /// -----------------------------------------
        /// ConstantForce Class
        /// -----------------------------------------
        vec3 get_direction(ConstantForce &const_force) { return const_force.direction; }
        double get_strength(ConstantForce &const_force) { return const_force.strength; }
        double get_start_time(ConstantForce &const_force) { return const_force.start_time; }
        double get_end_time(ConstantForce &const_force) { return const_force.end_time; }
        bool get_const_acc(ConstantForce &const_force) { return const_force.const_acceleration; }
    };
}

env::FriendClassForTests env_friend;


/// -----------------------------------------
/// Checkpointing tests
/// -----------------------------------------

// tests if a checkpoint file is written correctly.
TEST(IOTest, write_checkpointing_file_test) {
    env::Environment env;
    env.add_particle({0.5, 0.5, 0.5}, {0.5, 0.5, 0.5}, 1.5, 0, env::Particle::ALIVE);
    env.add_particle({1.5, 1.5, 1.5}, {1.5, 1.5, 1.5}, 1.5, 1, env::Particle::ALIVE);
    env.add_particle({2.5, 2.5, 2.5}, {2.5, 2.5, 2.5}, 2.5, 2, env::Particle::ALIVE);
    env.add_particle({3.5, 3.5, 3.5}, {3.5, 3.5, 3.5}, 3.5, 3, env::Particle::STATIONARY);
    env.build();

    env[0].old_force = {1, 2, 3};
    env[1].old_force = {4, 5, 6};
    env[2].old_force = {7, 8, 9};
    env[3].old_force = {10, 11, 12};

    io::CheckpointWriter checkpointWriter("write_test", "CheckpointTest");
    checkpointWriter.write_checkpoint_file(env, 1);

    std::ifstream file_to_check("CheckpointTest/write_test1.txt");
    ASSERT_TRUE(file_to_check.is_open());

    std::ifstream correct_file("../../testing/test_input_files/Checkpoint/CheckpointingTestFileWrite.txt");
    ASSERT_TRUE(correct_file.is_open());

    std::string check_line, correct_line;
    while (true) {
        bool getline_correct_file = static_cast<bool>(std::getline(correct_file, correct_line));
        bool getline_check_file = static_cast<bool>(std::getline(file_to_check, check_line));

        if (!getline_correct_file || !getline_check_file) {
            break;
        }

        EXPECT_EQ(check_line, correct_line);
    }

    EXPECT_EQ(file_to_check.eof(), correct_file.eof());

    correct_file.close();
    file_to_check.close();
}

// tests if force is read correctly from a checkpoint file
// (just checking force here, since other input is tested in read_particle_txt_file_test)
TEST(IOTest, read_checkpointing_file_test) {
    io::ProgramArguments args;
    io::read_file_txt("../../testing/test_input_files/Checkpoint/CheckpointingTestFileRead.txt", args);

    // particle 1
    EXPECT_EQ(args.env[0].force[0], 1);
    EXPECT_EQ(args.env[0].force[1], 1.5);
    EXPECT_EQ(args.env[0].force[2], 2);

    // particle 2
    EXPECT_EQ(args.env[1].force[0], 2.5);
    EXPECT_EQ(args.env[1].force[1], 3);
    EXPECT_EQ(args.env[1].force[2], 3.5);
}


/// -----------------------------------------
/// XML Input tests
/// -----------------------------------------

// tests if the arguments are read correctly from a xml file.
TEST(IOTest, read_arguments_xml_file_test) {
    io::ProgramArguments args;
    io::read_file_xml("../../testing/test_input_files/xml/InputTest1.xml", args);

    EXPECT_EQ(args.output_baseName, "output");
    EXPECT_EQ(args.duration, 20);
    EXPECT_EQ(args.dt, 0.0005);
    EXPECT_EQ(args.cutoff_radius, 2.5);
    EXPECT_EQ(args.write_freq, 100);
    EXPECT_EQ(args.parallel_strategy, 2);
}

// tests if the boundary info is read correctly from a xml file.
TEST(IOTest, read_boundary_xml_file_test) {
    io::ProgramArguments args;
    io::read_file_xml("../../testing/test_input_files/xml/InputTest1.xml", args);

    EXPECT_EQ(args.boundary.origin[0], 0);
    EXPECT_EQ(args.boundary.origin[1], 1);
    EXPECT_EQ(args.boundary.origin[2], 2);

    EXPECT_EQ(args.boundary.extent[0], 100);
    EXPECT_EQ(args.boundary.extent[1], 50);
    EXPECT_EQ(args.boundary.extent[2], 75);

    EXPECT_EQ(args.boundary.boundary_rules()[0], env::BoundaryRule::PERIODIC);
    EXPECT_EQ(args.boundary.boundary_rules()[1], env::BoundaryRule::PERIODIC);
    EXPECT_EQ(args.boundary.boundary_rules()[2], env::BoundaryRule::REPULSIVE_FORCE);
    EXPECT_EQ(args.boundary.boundary_rules()[3], env::BoundaryRule::VELOCITY_REFLECTION);
    EXPECT_EQ(args.boundary.boundary_rules()[4], env::BoundaryRule::OUTFLOW);
    EXPECT_EQ(args.boundary.boundary_rules()[5], env::BoundaryRule::OUTFLOW);

    EXPECT_EQ(env_friend.get_grid_constant(args.env), 2.5);
}

// tests if thermostat info is read correctly from a xml file.
TEST(IOTest, read_thermostat_xml_file_test) {
    io::ProgramArguments args;
    io::read_file_xml("../../testing/test_input_files/xml/InputTest6.xml", args);

    EXPECT_EQ(env_friend.get_init_temp(args.thermostat), 40);
    EXPECT_EQ(env_friend.get_target_temp(args.thermostat), 50);
    EXPECT_EQ(env_friend.get_max_temp_change(args.thermostat), 4);
    EXPECT_EQ(args.temp_adj_freq, 100);
}

// tests if constant force info is read correctly from a xml file.
TEST(IOTest, read_const_force_xml_test) {
    io::ProgramArguments args;
    io::read_file_xml("../../testing/test_input_files/xml/InputTest7.xml", args);

    EXPECT_EQ(args.external_forces.size(), 2);

    vec3 exp_direction = {0, 1, 0};
    EXPECT_EQ(env_friend.get_direction(args.external_forces[0]), exp_direction);
    EXPECT_EQ(env_friend.get_strength(args.external_forces[0]), -12.44);

    exp_direction = {1, 0, 0};
    EXPECT_EQ(env_friend.get_direction(args.external_forces[1]), exp_direction);
    EXPECT_EQ(env_friend.get_strength(args.external_forces[1]), 5);
    EXPECT_EQ(env_friend.get_start_time(args.external_forces[1]), 3);
    EXPECT_EQ(env_friend.get_end_time(args.external_forces[1]), 9);
    EXPECT_FALSE(env_friend.get_const_acc(args.external_forces[1]));
}

// tests if particle info is read correctly from a xml file.
TEST(IOTest, read_particle_xml_file_test) {
    io::ProgramArguments args;
    io::read_file_xml("../../testing/test_input_files/xml/InputTest2.xml", args);

    EXPECT_EQ(args.env.size(env::Particle::ALIVE | env::Particle::STATIONARY), 2);

    // particle 1
    vec3 exp_position = {1, 2, 3};
    vec3 exp_velocity = {4, 5, 6};
    EXPECT_EQ(args.env[0].position, exp_position);
    EXPECT_EQ(args.env[0].velocity, exp_velocity);
    EXPECT_EQ(args.env[0].mass, 1);
    EXPECT_EQ(args.env[0].type, 0);
    int state = args.env[0].state == env::Particle::ALIVE ? 1 : 0;
    EXPECT_EQ(state, 1);

    // particle 2
    exp_position = {7, 8, 9};
    exp_velocity = {10, 11, 12};
    EXPECT_EQ(args.env[1].position, exp_position);
    EXPECT_EQ(args.env[1].velocity, exp_velocity);
    EXPECT_EQ(args.env[1].mass, 2);
    EXPECT_EQ(args.env[1].type, 1);
    state = args.env[1].state == env::Particle::ALIVE ? 1 : 0;
    EXPECT_EQ(state, 0);
}

// tests if cuboid info is read correctly from a xml file.
TEST(IOTest, read_cuboid_xml_file_test) {
    io::ProgramArguments args;
    io::read_file_xml("../../testing/test_input_files/xml/InputTest3.xml", args);

    EXPECT_EQ(args.env.size(env::Particle::ALIVE), 60);
    EXPECT_EQ(args.env.size(env::Particle::STATIONARY), 6);

    // cuboid 1
    vec3 exp_position = {1, 2, 3};
    vec3 exp_velocity = {4, 5, 6};
    EXPECT_EQ(args.env[0].position, exp_position);
    EXPECT_EQ(args.env[0].velocity, exp_velocity);
    EXPECT_EQ(args.env[0].mass, 1);
    EXPECT_EQ(args.env[0].type, 0);

    exp_position = {3, 5, 7};
    EXPECT_EQ(args.env[59].position, exp_position);
    EXPECT_EQ(args.env[59].velocity, exp_velocity);
    EXPECT_EQ(args.env[59].mass, 1);
    EXPECT_EQ(args.env[59].type, 0);

    // particle 2
    exp_position = {19, 20, 21};
    exp_velocity = {7, 8, 9};
    EXPECT_EQ(args.env[60].position, exp_position);
    EXPECT_EQ(args.env[60].velocity, exp_velocity);
    EXPECT_EQ(args.env[60].mass, 2);
    EXPECT_EQ(args.env[60].type, 1);

    exp_position = {19, 21, 23};
    EXPECT_EQ(args.env[65].position, exp_position);
    EXPECT_EQ(args.env[65].velocity, exp_velocity);
    EXPECT_EQ(args.env[65].mass, 2);
    EXPECT_EQ(args.env[65].type, 1);
}

// tests if sphere info is read correctly from a xml file.
TEST(IOTest, read_sphere_xml_file_test) {
    io::ProgramArguments args;
    io::read_file_xml("../../testing/test_input_files/xml/InputTest4.xml", args);

    EXPECT_EQ(args.env.size(env::Particle::ALIVE), 123);
    EXPECT_EQ(args.env.size(env::Particle::STATIONARY), 33);

    // sphere 1
    vec3 exp_position = {10, 11, 12};
    vec3 exp_velocity = {4, 5, 6};
    EXPECT_EQ(args.env[61].position, exp_position);
    EXPECT_EQ(args.env[61].velocity, exp_velocity);
    EXPECT_EQ(args.env[61].mass, 4);
    EXPECT_EQ(args.env[61].type, 0);

    exp_position = {13, 11, 12};
    EXPECT_EQ(args.env[122].position, exp_position);
    EXPECT_EQ(args.env[122].velocity, exp_velocity);
    EXPECT_EQ(args.env[122].mass, 4);
    EXPECT_EQ(args.env[122].type, 0);

    // particle 2
    exp_position = {30, 31, 32};
    exp_velocity = {1, 2, 3};
    EXPECT_EQ(args.env[139].position, exp_position);
    EXPECT_EQ(args.env[139].velocity, exp_velocity);
    EXPECT_EQ(args.env[139].mass, 2);
    EXPECT_EQ(args.env[139].type, 1);

    exp_position = {32, 31, 32};
    EXPECT_EQ(args.env[155].position, exp_position);
    EXPECT_EQ(args.env[155].velocity, exp_velocity);
    EXPECT_EQ(args.env[155].mass, 2);
    EXPECT_EQ(args.env[155].type, 1);
}

// tests if membrane info is read correctly from a xml file.
TEST(IOTest, read_membrane_xml_file_test) {
    io::ProgramArguments args;
    io::read_file_xml("../../testing/test_input_files/xml/InputTest5.xml", args);

    EXPECT_EQ(args.env.size(), 100);

    vec3 exp_position = {1, 2, 3};
    vec3 exp_velocity = {4, 5, 6};
    EXPECT_EQ(args.env[0].position, exp_position);
    EXPECT_EQ(args.env[0].velocity, exp_velocity);
    EXPECT_EQ(args.env[0].mass, 1);
    EXPECT_EQ(args.env[0].type, 0);

    exp_position = {10, 11, 3};
    EXPECT_EQ(args.env[99].position, exp_position);
    EXPECT_EQ(args.env[99].velocity, exp_velocity);
    EXPECT_EQ(args.env[99].mass, 1);
    EXPECT_EQ(args.env[99].type, 0);
}



/// -----------------------------------------
/// TXT Input tests
/// -----------------------------------------

// tests if the arguments are read correctly from a txt file.
TEST(IOTest, read_arguments_txt_file_test) {
    io::ProgramArguments args;
    io::read_file_txt("../../testing/test_input_files/txt/InputTest1.txt", args);

    EXPECT_EQ(args.output_baseName, "output");
    EXPECT_EQ(args.duration, 20);
    EXPECT_EQ(args.dt, 0.0005);
    EXPECT_EQ(args.cutoff_radius, 2.5);
    EXPECT_EQ(args.write_freq, 100);
    EXPECT_EQ(args.parallel_strategy, 2);
}

// tests if the boundary info is read correctly from a txt file.
TEST(IOTest, read_boundary_txt_file_test) {
    io::ProgramArguments args;
    io::read_file_txt("../../testing/test_input_files/txt/InputTest1.txt", args);

    EXPECT_EQ(args.boundary.origin[0], 0);
    EXPECT_EQ(args.boundary.origin[1], 1);
    EXPECT_EQ(args.boundary.origin[2], 2);

    EXPECT_EQ(args.boundary.extent[0], 100);
    EXPECT_EQ(args.boundary.extent[1], 50);
    EXPECT_EQ(args.boundary.extent[2], 75);

    EXPECT_EQ(args.boundary.boundary_rules()[0], env::BoundaryRule::PERIODIC);
    EXPECT_EQ(args.boundary.boundary_rules()[1], env::BoundaryRule::PERIODIC);
    EXPECT_EQ(args.boundary.boundary_rules()[2], env::BoundaryRule::VELOCITY_REFLECTION);
    EXPECT_EQ(args.boundary.boundary_rules()[3], env::BoundaryRule::VELOCITY_REFLECTION);
    EXPECT_EQ(args.boundary.boundary_rules()[4], env::BoundaryRule::OUTFLOW);
    EXPECT_EQ(args.boundary.boundary_rules()[5], env::BoundaryRule::OUTFLOW);

    EXPECT_EQ(env_friend.get_grid_constant(args.env), 2.5);
}

// tests if thermostat info is read correctly from a txt file.
TEST(IOTest, read_thermostat_txt_file_test) {
    io::ProgramArguments args;
    io::read_file_txt("../../testing/test_input_files/txt/InputTest6.txt", args);

    EXPECT_EQ(env_friend.get_init_temp(args.thermostat), 40);
    EXPECT_EQ(env_friend.get_target_temp(args.thermostat), 50);
    EXPECT_EQ(env_friend.get_max_temp_change(args.thermostat), 4);
    EXPECT_EQ(args.temp_adj_freq, 100);
}

// tests if constant force info is read correctly from a txt file.
TEST(IOTest, read_const_force_txt_test) {
    io::ProgramArguments args;
    io::read_file_txt("../../testing/test_input_files/txt/InputTest7.txt", args);

    EXPECT_EQ(args.external_forces.size(), 2);

    vec3 exp_direction = {0, 1, 0};
    EXPECT_EQ(env_friend.get_direction(args.external_forces[0]), exp_direction);
    EXPECT_EQ(env_friend.get_strength(args.external_forces[0]), -12.44);

    exp_direction = {1, 0, 0};
    EXPECT_EQ(env_friend.get_direction(args.external_forces[1]), exp_direction);
    EXPECT_EQ(env_friend.get_strength(args.external_forces[1]), 5);
    EXPECT_EQ(env_friend.get_start_time(args.external_forces[1]), 3);
    EXPECT_EQ(env_friend.get_end_time(args.external_forces[1]), 9);
    EXPECT_FALSE(env_friend.get_const_acc(args.external_forces[1]));
}

// tests if particle info is read correctly from a txt file.
TEST(IOTest, read_particle_txt_file_test) {
    io::ProgramArguments args;
    io::read_file_txt("../../testing/test_input_files/txt/InputTest2.txt", args);

    EXPECT_EQ(args.env.size(env::Particle::ALIVE | env::Particle::STATIONARY), 2);

    // particle 1
    vec3 exp_position = {1, 2, 3};
    vec3 exp_velocity = {4, 5, 6};
    EXPECT_EQ(args.env[0].position, exp_position);
    EXPECT_EQ(args.env[0].velocity, exp_velocity);
    EXPECT_EQ(args.env[0].mass, 1);
    EXPECT_EQ(args.env[0].type, 0);
    int state = args.env[0].state == env::Particle::ALIVE ? 1 : 0;
    EXPECT_EQ(state, 1);

    // particle 2
    exp_position = {7, 8, 9};
    exp_velocity = {10, 11, 12};
    EXPECT_EQ(args.env[1].position, exp_position);
    EXPECT_EQ(args.env[1].velocity, exp_velocity);
    EXPECT_EQ(args.env[1].mass, 2);
    EXPECT_EQ(args.env[1].type, 1);
    state = args.env[1].state == env::Particle::ALIVE ? 1 : 0;
    EXPECT_EQ(state, 0);
}

// tests if cuboid info is read correctly from a txt file.
TEST(IOTest, read_cuboid_txt_file_test) {
    io::ProgramArguments args;
    io::read_file_txt("../../testing/test_input_files/txt/InputTest3.txt", args);

    EXPECT_EQ(args.env.size(env::Particle::ALIVE), 60);
    EXPECT_EQ(args.env.size(env::Particle::STATIONARY), 6);

    // cuboid 1
    vec3 exp_position = {1, 2, 3};
    vec3 exp_velocity = {4, 5, 6};
    EXPECT_EQ(args.env[0].position, exp_position);
    EXPECT_EQ(args.env[0].velocity, exp_velocity);
    EXPECT_EQ(args.env[0].mass, 1);
    EXPECT_EQ(args.env[0].type, 0);

    exp_position = {3, 5, 7};
    EXPECT_EQ(args.env[59].position, exp_position);
    EXPECT_EQ(args.env[59].velocity, exp_velocity);
    EXPECT_EQ(args.env[59].mass, 1);
    EXPECT_EQ(args.env[59].type, 0);

    // particle 2
    exp_position = {19, 20, 21};
    exp_velocity = {7, 8, 9};
    EXPECT_EQ(args.env[60].position, exp_position);
    EXPECT_EQ(args.env[60].velocity, exp_velocity);
    EXPECT_EQ(args.env[60].mass, 2);
    EXPECT_EQ(args.env[60].type, 1);

    exp_position = {19, 21, 23};
    EXPECT_EQ(args.env[65].position, exp_position);
    EXPECT_EQ(args.env[65].velocity, exp_velocity);
    EXPECT_EQ(args.env[65].mass, 2);
    EXPECT_EQ(args.env[65].type, 1);
}

// tests if sphere info is read correctly from a txt file.
TEST(IOTest, read_sphere_txt_file_test) {
    io::ProgramArguments args;
    io::read_file_txt("../../testing/test_input_files/txt/InputTest4.txt", args);

    EXPECT_EQ(args.env.size(env::Particle::ALIVE), 123);
    EXPECT_EQ(args.env.size(env::Particle::STATIONARY), 33);

    // sphere 1
    vec3 exp_position = {10, 11, 12};
    vec3 exp_velocity = {4, 5, 6};
    EXPECT_EQ(args.env[61].position, exp_position);
    EXPECT_EQ(args.env[61].velocity, exp_velocity);
    EXPECT_EQ(args.env[61].mass, 4);
    EXPECT_EQ(args.env[61].type, 0);

    exp_position = {13, 11, 12};
    EXPECT_EQ(args.env[122].position, exp_position);
    EXPECT_EQ(args.env[122].velocity, exp_velocity);
    EXPECT_EQ(args.env[122].mass, 4);
    EXPECT_EQ(args.env[122].type, 0);

    // particle 2
    exp_position = {30, 31, 32};
    exp_velocity = {1, 2, 3};
    EXPECT_EQ(args.env[139].position, exp_position);
    EXPECT_EQ(args.env[139].velocity, exp_velocity);
    EXPECT_EQ(args.env[139].mass, 2);
    EXPECT_EQ(args.env[139].type, 1);

    exp_position = {32, 31, 32};
    EXPECT_EQ(args.env[155].position, exp_position);
    EXPECT_EQ(args.env[155].velocity, exp_velocity);
    EXPECT_EQ(args.env[155].mass, 2);
    EXPECT_EQ(args.env[155].type, 1);
}

// tests if membrane info is read correctly from a txt file.
TEST(IOTest, read_membrane_txt_file_test) {
    io::ProgramArguments args;
    io::read_file_txt("../../testing/test_input_files/txt/InputTest5.txt", args);

    EXPECT_EQ(args.env.size(), 100);

    vec3 exp_position = {1, 2, 3};
    vec3 exp_velocity = {4, 5, 6};
    EXPECT_EQ(args.env[0].position, exp_position);
    EXPECT_EQ(args.env[0].velocity, exp_velocity);
    EXPECT_EQ(args.env[0].mass, 1);
    EXPECT_EQ(args.env[0].type, 0);

    exp_position = {10, 11, 3};
    EXPECT_EQ(args.env[99].position, exp_position);
    EXPECT_EQ(args.env[99].velocity, exp_velocity);
    EXPECT_EQ(args.env[99].mass, 1);
    EXPECT_EQ(args.env[99].type, 0);
}