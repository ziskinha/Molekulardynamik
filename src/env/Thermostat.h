#pragma once

#include <limits>
#define INF_TEMP std::numeric_limits<double>::infinity()
#define NO_TEMP (-1)

namespace md::env {
    class Environment;

    class Thermostat {
    public:
        explicit Thermostat(double init_T=NO_TEMP, double target_T=NO_TEMP, double dT=INF_TEMP);

        void init(double init_T, double target_T, double dT);
        void set_initial_temperature(Environment & env) const;
        void adjust_temperature(Environment & env) const;

    private:
        double init_temp;
        double target_temp;
        double max_temp_change;
    };
}



