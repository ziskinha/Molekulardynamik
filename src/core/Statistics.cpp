#include "Statistics.h"

#include "io/Logger/Logger.h"

#include <cmath>
#include <fstream>               
#include "env/Particle.h"

namespace md::core {
    NanoFlowStatistics::NanoFlowStatistics(const int compute_freq, const int n_bins):
        Statistics(compute_freq), n_bins(n_bins), velocityWriter(n_bins,"velocityStats"), densityWriter(n_bins, "densityStats") {}

    void NanoFlowStatistics::compute(const env::Environment& env, double time) {
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
            if(avg_density[i]!=0){
            avg_velocity[i] /= avg_density[i];
            }else{
                avg_velocity[i]=0.0;
            }
            avg_density[i] /= bin_volume;
        }

        // TODO write averages to csv file
        velocityWriter.writeData(avg_velocity, time);
        densityWriter.writeData(avg_density, time);
        SPDLOG_DEBUG("avg_velocity: [{}]", fmt::join(avg_velocity, ", "));
        SPDLOG_DEBUG("avg_density: [{}]", fmt::join(avg_density, ", "));
    }   
}
