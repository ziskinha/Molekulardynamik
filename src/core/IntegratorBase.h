#pragma once

#include "env/Environment.h"
#include "io/IOStrategy.h"

namespace md::Integrator {
    /**
     * @brief Manages the main simulation loop.
     */
    class IntegratorBase {
       public:
        /**
         * @brief Constructs a IntegratorBase object with a reference to a ParticleContainer and OutputWriter.
         * @param environment physical system to be simulated
         * @param writer used to log/plot particle data
         */
        IntegratorBase(env::Environment& environment, std::unique_ptr<io::OutputWriterBase> writer);

        /**
         * @brief Virtual destructor.
         */
        virtual ~IntegratorBase() = default;

        /**
         * @brief Runs the simulation over a given time interval with specified step size and output frequency.
         * @param start_time
         * @param end_time
         * @param dt Δt The time increment for each simulation step.
         * @param write_freq The frequency with which the data is written to output.
         */
        void simulate(double start_time, double end_time, double dt, unsigned int write_freq);

        /**
         * @brief Runs the simulation over a given time interval with specified step size and output frequency.
         * (Used for tests).
         * @param start_time
         * @param end_time
         * @param dt Δt The time increment for each simulation step.
         */
        void simulate_without_writer(double start_time, double end_time, double dt);

        /**
         * @brief Runs the simulation in benchmark mode. Benchmarking the overall runtime of the simulation.
         * @param start_time
         * @param end_time
         * @param dt
         * @param file_name
         * @param repititions
         */
        void benchmark_overall(const double start_time, const double end_time, const double dt, const std::string& file_name, int repetitions);

        /**
         * @brief Runs the simulation in benchmark mode. Benchmarking the runtime per iteration.
         * @param start_time
         * @param end_time
         * @param dt
         * @param file_name
         * @param repetitions
         */
        void benchmark_iterations(const double start_time, const double end_time, const double dt, const std::string& file_name, int repetitions);

        /**
         * @brief Runs the simulation in benchmark mode,
         * @param start_time
         * @param end_time
         * @param dt
         * @param file_name
         */
        void benchmark_simulate(const double start_time, const double end_time, const double dt, const std::string &file_name);

       protected:
        /**
         * @brief Abstract method for performing a single simulation step.
         * @param dt Δt The time increment for each simulation step.
         */
        virtual void simulation_step(double dt) = 0;

        env::Environment& env;  ///< Reference to the environment.

       private:
        std::unique_ptr<io::OutputWriterBase> writer;  ///< The output writer.
    };
}  // namespace md::Integrator
