#pragma once

#include <limits>

namespace md::env {
    class Environment;

    /**
     * @brief Manages temperature initialization and adjustment.
     */
    class Thermostat {
    public:
        static constexpr double INF_TEMP = std::numeric_limits<double>::max();
        static constexpr double NO_TEMP = -1;

        /**
         * @brief Constructs a Thermostat object.
         * @param init_T The initial temperature (default: NO_TEMP).
         * @param target_T The target temperature (default: NO_TEMP).
         * @param dT The maximal absolute temperature change allowed for one application of the thermostat
         * (default: INF_TEMP).
         */
        explicit Thermostat(double init_T=NO_TEMP, double target_T=NO_TEMP, double dT=INF_TEMP);

        /**
         * @brief Initializes the Thermostat.
         * @param init_T The initial temperature.
         * @param target_T The target temperature.
         * @param dT The maximal absolute temperature change allowed for one application of the thermostat.
         */
        void init(double init_T, double target_T, double dT);

        /**
         * @brief Sets the initial temperature of all particles in the environment.
         * @param env The environment containing the particles.
         */
        void set_initial_temperature(Environment & env) const;

        /**
         * @brief Adjusts the current temperature of the environment towards the target temperature.
         * @param env The environment containing the particles.
         */
        void adjust_temperature(Environment & env) const;

        /**
         * @brief Used for testing.
         */
        friend class FriendClassForTests;
    private:
        double init_temp;        ///< The initial temperature.
        double target_temp;      ///< The target temperature.
        double max_temp_change;  ///< The maximal absolute temperature change allowed for one application of the thermostat.
    };
} // namespace md::env



