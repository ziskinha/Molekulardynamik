#pragma once

#include <vector>
#include <limits>
#include "Particle.h"

#define INF_TEMP std::numeric_limits<double>::infinity()
#define NO_TEMP (-1)

namespace md::env {
    class Thermostat {
    public:
        explicit Thermostat(double init_T=NO_TEMP, double target_T=NO_TEMP, double dT=INF_TEMP, int dimension=2);

        double temperature(std::vector<Particle> & particles) const;
        void set_initial_temperature(std::vector<Particle> & particles) const;
        void change_temperature(std::vector<Particle> & particles) const;

    private:
        int dimension;
        double init_temp;
        double target_temp;
        double max_temp_change;
    };
}



