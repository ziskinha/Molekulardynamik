#include "Thermostat.h"

#include "utils/ArrayUtils.h"
#include "utils/MaxwellBoltzmannDistribution.h"


namespace md::env {
    Thermostat::Thermostat(const double init_T, const double target_T, const double dT, const int dimension):
    dimension(dimension), init_temp(init_T), target_temp(target_T), max_temp_change(dT)
    {

    }

    double Thermostat::temperature(std::vector<Particle>& particles) const {
        double energy = 0;
        for (auto & particle : particles) {
            if (particle.state == Particle::ALIVE) {
                energy += 0.5 * particle.mass * ArrayUtils::L2NormSquared(particle.velocity);
            }
        }

        return dimension * energy/3;
    }

    void Thermostat::set_initial_temperature(std::vector<Particle>& particles) const {
        if (init_temp == NO_TEMP) return;
        for (auto & particle : particles) {
            particle.velocity = maxwellBoltzmannDistributedVelocity(sqrt(init_temp/particle.mass), dimension);
        }
    }

    void Thermostat::change_temperature(std::vector<Particle>& particles) const {
        if (target_temp == NO_TEMP) return;

        const double current_temp = temperature(particles);
        const double diff = target_temp - current_temp;
        const double new_temp = current_temp + std::clamp(diff, -max_temp_change, max_temp_change);
        const double beta = sqrt(new_temp/current_temp);
        if (new_temp == current_temp) return;

        for (auto & particle : particles) {
            if (particle.state == Particle::ALIVE) {
                particle.velocity = beta * particle.velocity;
            }
        }
    }
}
