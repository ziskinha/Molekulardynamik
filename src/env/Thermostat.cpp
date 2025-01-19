#include "Thermostat.h"

#include "utils/ArrayUtils.h"
#include "utils/MaxwellBoltzmannDistribution.h"
#include "env/Environment.h"

#include "io/Logger/Logger.h"

namespace md::env {
    Thermostat::Thermostat(const double init_T, const double target_T, const double dT) :
          init_temp(init_T),
          target_temp(target_T),
          max_temp_change(dT) {}

    void Thermostat::init(const double init_T, const double target_T, const double dT) {
        init_temp = init_T;
        target_temp = target_T;
        max_temp_change = dT;
    }

    void Thermostat::set_initial_temperature(Environment& env) const {
        if (init_temp == NO_TEMP) return;
        for (auto & particle : env.particles()) {
            particle.velocity = maxwellBoltzmannDistributedVelocity(sqrt(init_temp/particle.mass), env.dim());
        }
    }

    void Thermostat::adjust_temperature(Environment& env) const {
        if (target_temp == NO_TEMP) return;

        const double current_temp = env.temperature();
        const double diff = target_temp - current_temp;
        const double new_temp = current_temp + std::clamp(diff, -max_temp_change, max_temp_change);
        const double beta = sqrt(new_temp/current_temp);

        SPDLOG_DEBUG("current temp: {}, new temp: {}, beta {}", current_temp, new_temp, beta);

        if (new_temp == current_temp) return;

        for (auto & particle : env.particles(GridCell::INSIDE, Particle::ALIVE)) {
            particle.velocity = beta * particle.velocity;
        }
    }

} // namespace md::env
