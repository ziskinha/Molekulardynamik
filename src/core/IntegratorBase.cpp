
#include "IntegratorBase.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "io/Logger.h"

#if SPDLOG_ACTIVE_LEVEL != SPDLOG_LEVEL_OFF
#define SHOW_PROGRESS(current, total)                                                         \
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
    IntegratorBase::IntegratorBase(env::Environment& system, std::unique_ptr<io::OutputWriterBase> writer)
        : environment(system), writer(std::move(writer)) {}

    void IntegratorBase::simulate(const double start_time, const double end_time, const double dt,
                                  const unsigned int write_freq, const bool benchmark) {
        int i = 0;
        const int total_steps = static_cast<int>((end_time - start_time) / dt);
        SPDLOG_INFO("Simulation started");

        if (benchmark) {
            long duration_sum = 0;
            int repetitions = 150;
            SPDLOG_INFO("Benchmarking enabled. Subsequent logging messages during simulation are disabled.");

            for (int k = 1; k <= repetitions; k++) {
                spdlog::set_level(spdlog::level::off);
                auto start = std::chrono::high_resolution_clock::now();

                for (double t = start_time; t < end_time; t += dt) {
                    simulation_step(dt);
                }
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                spdlog::set_level(spdlog::level::info);
                SPDLOG_INFO("Finished {}. benchmark simulation, out of {}", k, repetitions);
                SPDLOG_INFO("Execution time: {} milliseconds", duration);
                SPDLOG_INFO("Number of particles: {}", environment.size());
                SPDLOG_INFO("Number of steps: {}", total_steps);
                duration_sum += duration;
            }
            SPDLOG_INFO("Average execution time: {} milliseconds", duration_sum / repetitions);

        } else {
            for (double t = start_time; t < end_time; t += dt, i++) {
                simulation_step(dt);
                if (i % write_freq == 0) {
                    if (writer != nullptr) {
                        SPDLOG_DEBUG("Plotting particles @ iteration {}, time {}", i, t);
                        writer->plot_particles(environment, i);
                    }
                }

                SHOW_PROGRESS(i, total_steps);
            }
        }

        SPDLOG_INFO("Simulation ended");
    }
}  // namespace md::Integrator