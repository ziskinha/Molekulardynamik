
#include "IntegratorBase.h"

#include <chrono>
#include <iomanip>
#include <iostream>

#include "io/Logger/Logger.h"
#include <chrono>

#if SPDLOG_ACTIVE_LEVEL == SPDLOG_LEVEL_INFO
#define SHOW_PROGRESS(current, total)                                                      \
 do {                                                                                      \
     constexpr int bar_width = 50;                                                         \
     const float progress = static_cast<float>(current) / (total);                         \
                                                                                           \
     std::cout << "\r[";                                                                   \
     int pos = bar_width * progress;                                                       \
     for (int i = 0; i < bar_width; ++i) {                                                 \
         if (i < pos)                                                                      \
             std::cout << "=";                                                             \
         else if (i == pos)                                                                \
             std::cout << ">";                                                             \
         else                                                                              \
             std::cout << " ";                                                             \
     }                                                                                     \
     std::cout << "] " << std::fixed << std::setprecision(2) << (progress * 100.0) << "%"; \
     std::cout.flush();                                                                    \
     if ((current) == (total)) {                                                           \
         std::cout << std::endl;                                                           \
     }                                                                                     \
 } while (0)
#else
#define SHOW_PROGRESS(current, total) \
 do {                              \
 } while (0)
#endif

namespace md::Integrator {
    IntegratorBase::IntegratorBase(
        env::Environment& environment,
        std::unique_ptr<io::OutputWriterBase> writer,
        const env::Thermostat & thermostat)
        : env(environment), thermostat(thermostat), temp_adjust_freq(0), writer(std::move(writer)) {}

    void IntegratorBase::simulate(const double start_time, const double end_time, const double dt, double& modifications,
                                  const unsigned int write_freq, const unsigned int temp_adj_freq) {
        auto checkpoint_writer = io::create_checkpoint_writer();
        temp_adjust_freq = temp_adj_freq;
        int step = 0;
        const int total_steps = static_cast<int>((end_time - start_time) / dt);
        SPDLOG_INFO("Simulation started");

        for (double t = start_time; t < end_time; t += dt, step++) {
            simulation_step(step, dt, modifications);
            if (step % write_freq == 0 && writer) {
                SPDLOG_DEBUG("Plotting particles @ iteration {}, time {}", step, t);
                writer->plot_particles(env, step);
            }
            SHOW_PROGRESS(step, total_steps);
        }

        checkpoint_writer->write_checkpoint_file(env, 1);
        SPDLOG_INFO("Simulation ended");
    }

    /// -----------------------------------------
    /// \brief Benchmark functions
    /// -----------------------------------------

    void IntegratorBase::benchmark(double start_time, double end_time, double dt, unsigned int temp_adj_freq) {
        temp_adjust_freq = temp_adj_freq;

        unsigned long long total_micros = 0;
        // TODO delete
        double modifications=0;
        int step = 0;
        for (double t = start_time; t < end_time; t += dt, step++) {
            auto start = std::chrono::high_resolution_clock::now();
            simulation_step(step, dt, modifications);
            auto end = std::chrono::high_resolution_clock::now();
            total_micros += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        }
        const double avg_step_time = static_cast<double>(total_micros) / static_cast<double>(step+1);

        // Using std::cout instead of logging, as logging should be disabled during benchmarking.
        std::cout << "Total execution time:" << total_micros/1000.0 << " ms" << std::endl;
        std::cout << "Average execution time per step: " << avg_step_time/1000.0 << " ms" << std::endl;
        std::cout << "Number of particles: " << env.size() << std::endl;
    }
}  // namespace md::Integrator