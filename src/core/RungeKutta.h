#pragma once

#include "env/Environment.h"
#include "IntegratorBase.h"

namespace md::Integrator {
    /**
     * @brief Implements the calculation of Runge-Kutta.
     */
    class RungeKutta : public IntegratorBase {
       public:
        /**
         * @brief Constructs a RungeKutta object with specified force calculation.
         * @param environemt
         * @param writer writes the output to disk.
         */
        RungeKutta(Environment& environemt, std::unique_ptr<io::OutputWriterBase> writer);

       private:
        /**
         * @brief Performs a single step of the Runge Kutta.
         * @param dt Δt The time increment for each simulation step.
         */
        void simulation_step(unsigned step, double dt) override;

        /**
         * @brief Computes the forces of the particles.
         */
        void compute_forces() const;
    };
}  // namespace md::Integrator
