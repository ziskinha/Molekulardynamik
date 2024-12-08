#pragma once

#include <limits>
#include "env/Environment.h"
#include "env/Thermostat.h"
#include "io/IOStrategy.h"
#define MAX_UNS std::numeric_limits<unsigned int>::max()

namespace md::Integrator {
    /**
     * @brief Manages the main simulation loop.
     */
    class IntegratorBase {
       public:
        /**
         * @brief Constructs a IntegratorBase object with a reference to a ParticleContainer and OutputWriter.
         * @param environment physical system to be simulated
         * @param thermostat
         * @param writer used to log/plot particle data
         */
        explicit IntegratorBase(env::Environment& environment, std::unique_ptr<io::OutputWriterBase> writer = nullptr,
            const env::Thermostat & thermostat = env::Thermostat());

        /**
         * @brief Virtual destructor.
         */
        virtual ~IntegratorBase() = default;

        /**
         * @brief Runs the simulation over a given time interval with specified step size and output frequency.
         * @param start_time
         * @param end_time
         * @param dt Δt The time increment for each simulation step.
         * @param temp_adj_freq number of time steps between periodic temperature adjustments
         * @param write_freq The frequency with which the data is written to output.
         */
        void simulate(double start_time, double end_time, double dt, unsigned int write_freq = 1000, unsigned int temp_adj_freq = MAX_UNS);

        /**
         * @brief Runs the simulation in benchmark mode. Benchmarking the overall runtime of the simulation.
         * @param start_time
         * @param end_time
         * @param dt
         * @param repetitions
         */
        void benchmark_overall(double start_time, double end_time, double dt,  int repetitions);

        /**
         * @brief Runs the simulation in benchmark mode. Benchmarking the runtime per iteration.
         * @param start_time
         * @param end_time
         * @param dt
         * @param repetitions
         */
        void benchmark_iterations(double start_time, double end_time, double dt, int repetitions);

        /**
         * @brief Runs the simulation in benchmark mode,
         * @param start_time
         * @param end_time
         * @param dt
         */
        void benchmark_simulate(double start_time, double end_time, double dt);

       protected:
        /**
         * @brief Abstract method for performing a single simulation step.
         * @param step current simulation step
         * @param dt Δt The time increment for each simulation step.
         */
        virtual void simulation_step(unsigned int step, double dt) = 0;

        env::Environment& env;  ///< Reference to the environment.
        const env::Thermostat thermostat; ///< Thermostat to adjust temperature of the environment
        unsigned int temp_adjust_freq;

       private:
        std::unique_ptr<io::OutputWriterBase> writer;  ///< The output writer.
    };
}  // namespace md::Integrator
