

#pragma once

#include "IntegratorBase.h"
#include "force.h"

namespace md::Integrator {
    /**
     * @brief Implements the calculation of Runge-Kutta
     */
    class RungeKutta : public IntegratorBase {
       public:
        /**
         * @brief Constructs a RungeKutta object with specified force calculation.
         * @param particles contains the to-be-simulated particles
         * @param force_func Defines the force function which is being used.
         * @param writer writes the output to disk
         */
        RungeKutta(ParticleContainer& particles, force::ForceFunc force_func,
                   std::unique_ptr<io::OutputWriterBase> writer);

       private:
        /**
         * @brief Performs a single step of the Runge Kutta.
         * @param dt Δt The time increment for each simulation step.
         */
        void simulation_step(double dt) override;
        void compute_forces() const;
        force::ForceFunc force_func;
    };
}  // namespace md::Integrator
