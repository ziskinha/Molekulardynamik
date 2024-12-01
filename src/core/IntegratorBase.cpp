
#include "IntegratorBase.h"

#include <chrono>
#include <iomanip>
#include <iostream>

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
        : env(system), writer(std::move(writer)){}

    void IntegratorBase::simulate(const double start_time, const double end_time, const double dt,
                                  const unsigned int write_freq) {
        int i = 0;
        const int total_steps = static_cast<int>((end_time - start_time) / dt);
        SPDLOG_INFO("Simulation started");

        for (double t = start_time; t < end_time; t += dt, i++) {
            simulation_step(dt);
            if (i % write_freq == 0) {
                if (writer != nullptr) {
                    SPDLOG_DEBUG("Plotting particles @ iteration {}, time {}", i, t);
                    writer->plot_particles(env, i);
                }
            }
        }

        SPDLOG_INFO("Simulation ended");
    }


    /// -----------------------------------------
    /// \brief Benchmark functions
    /// -----------------------------------------
    void IntegratorBase::benchmark_overall(const double start_time, const double end_time, const double dt,
                                           const std::string &file_name, int repetitions) {
        long duration_sum = 0;

        for (int k = 1; k <= repetitions; k++) {
            spdlog::set_level(spdlog::level::off);
            env::Environment new_env;
            md::io::read_file(file_name, new_env);
            new_env.build();

            auto start = std::chrono::high_resolution_clock::now();

            for (double t = start_time; t < end_time; t += dt) {
                simulation_step(dt);
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            spdlog::set_level(spdlog::level::info);
            SPDLOG_INFO("Finished {}. benchmark simulation, out of {}", k, repetitions);
            SPDLOG_INFO("Execution time: {} ms", duration);
            SPDLOG_INFO("Number of particles: {}", new_env.size());
            duration_sum += duration;
        }
        SPDLOG_INFO("Average execution time: {} ms", duration_sum / repetitions);
    }

    void IntegratorBase::benchmark_iterations(const double start_time, const double end_time, const double dt,
                                              const std::string &file_name, int repetitions) {
        double avg_duration_sum = 0.0;

        for (int k = 1; k <= repetitions; k++) {
            spdlog::set_level(spdlog::level::off);
            auto duration_sum = 0.0;

            env::Environment new_env;
            md::io::read_file(file_name, new_env);
            new_env.build();

            for (double t = start_time; t < end_time; t += dt) {
                auto start = std::chrono::high_resolution_clock::now();
                simulation_step(dt);
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                duration_sum += duration;
            }

            spdlog::set_level(spdlog::level::info);

            double avg_time = duration_sum / (end_time / dt);
            avg_duration_sum += avg_time;
            SPDLOG_INFO("Average loop time of the {}. repetition: {} ms", k, avg_time);
        }

        SPDLOG_INFO("Average loop time of {} repetitions: {} ms", repetitions, avg_duration_sum /repetitions);
    }

    void IntegratorBase::benchmark_simulate(const double start_time, const double end_time, const double dt,
                                            const std::string &file_name) {
        SPDLOG_INFO("Benchmarking enabled. Subsequent logging messages during simulation are disabled.");
        int repetitions = 10;
        //benchmark_overall(start_time, end_time, dt, repetitions);
        benchmark_iterations(start_time, end_time, dt, file_name, repetitions);
    };
}  // namespace md::Integrator