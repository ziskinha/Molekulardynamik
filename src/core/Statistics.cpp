#include "Statistics.h"

#include <spdlog/spdlog.h>

#include <cmath>

#include "env/Particle.h"

namespace md::core {
    NanoFlowStatistics::NanoFlowStatistics(const int compute_freq, const int n_bins):
        Statistics(compute_freq), n_bins(n_bins) {}

    void NanoFlowStatistics::compute(const env::Environment& env) {
        const double bin_width = env.extent()[0] / static_cast<double>(n_bins);
        const double bin_volume = env.extent()[1] * env.extent()[2] * bin_width;

        std::vector avg_velocity(n_bins, 0.0);
        std::vector avg_density(n_bins, 0.0);

        for (const auto & p : env.particles()) {
            const int idx = std::floor(p.position[0] / bin_width);
            avg_velocity[idx] += p.velocity[1]; // velocity in y direction
            avg_density[idx]++;
        }

        for (int i = 0; i < n_bins; i++) {
            avg_velocity[i] /= avg_density[i];
            avg_density[i] /= bin_volume;
        }

        // TODO write averages to csv file

        SPDLOG_DEBUG("avg_velocity: {}", avg_velocity);
        SPDLOG_DEBUG("avg_density: {}", avg_density);
    }
}
