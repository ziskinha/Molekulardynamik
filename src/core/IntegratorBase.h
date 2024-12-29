#pragma once

#include <limits>
#include "env/Environment.h"
#include "effects/Thermostat.h"
#include "io/IOStrategy.h"
#include "effects/ConstantForce.h"
#define NEVER std::numeric_limits<unsigned int>::max()

namespace md::Integrator {

    struct IntegratorCreateInfo {
        explicit IntegratorCreateInfo(env::Environment & env) : environment(env) {}
        env::Environment & environment;
        std::unique_ptr<io::OutputWriterBase> writer = nullptr;
        std::unique_ptr<io::CheckpointWriter> checkpoint_writer = nullptr;
        env::Thermostat thermostat = env::Thermostat();
        std::vector<env::ConstantForce> external_forces = {};
    };

    /**
     * @brief Manages the main simulation loop.
     */
    class IntegratorBase {
       public:

        explicit IntegratorBase(IntegratorCreateInfo create_info);

        /**
         * @brief Constructs a IntegratorBase object with a reference to a ParticleContainer and OutputWriter.
         * @param environment physical system to be simulated.
         * @param checkpoint_writer
         * @param thermostat
         * @param external_forces
         * @param writer used to log/plot particle data.
         */
        explicit IntegratorBase(env::Environment& environment, std::unique_ptr<io::OutputWriterBase> writer = nullptr,
                                std::unique_ptr<io::CheckpointWriter> checkpoint_writer = nullptr,
                                const env::Thermostat & thermostat = env::Thermostat(),
                                const std::vector<env::ConstantForce> & external_forces = {});

        /**
         * @brief Virtual destructor.
         */
        virtual ~IntegratorBase() = default;

        /**
         * @brief Runs the simulation over a given time interval with specified step size and output frequency.
         * @param start_time
         * @param end_time
         * @param dt Δt The time increment for each simulation step.
         * @param temp_adj_freq Number of time steps between periodic temperature adjustments.
         * @param write_freq The frequency with which the data is written to output.
         */
        void simulate(double start_time, double end_time, double dt, unsigned int write_freq = 1000, unsigned int temp_adj_freq = NEVER );

        /**
         * @brief Benchmarks the performance of the simulation. Calculates the total execution time and the average
         * execution time per step.
         * @param start_time
         * @param end_time
         * @param dt Δt The time increment for each simulation step.
         * @param temp_adj_freq Number of time steps between periodic temperature adjustments.
         */
        void benchmark(double start_time, double end_time, double dt, unsigned int temp_adj_freq = NEVER);

       protected:
        /**
         * @brief Abstract method for performing a single simulation step.
         * @param step current simulation step
         * @param dt Δt The time increment for each simulation step.
         */
        virtual void simulation_step(unsigned int step, double dt) {};

        env::Environment& env;            ///< Reference to the environment.
        const env::Thermostat thermostat; ///< Thermostat to adjust temperature of the environment
        unsigned int temp_adjust_freq;    ///< Number of time steps between periodic temperature adjustments.
        std::vector<env::ConstantForce> external_forces;

       private:
        std::unique_ptr<io::OutputWriterBase> writer;  ///< The output writer.
        std::unique_ptr<io::CheckpointWriter> checkpoint_writer;  ///< The output checkpoint writer.
    };
}  // namespace md::Integrator
