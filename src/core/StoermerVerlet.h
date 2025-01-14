#pragma once

#include "IntegratorBase.h"

namespace md::Integrator {

    /**
     * @brief Implements the calculation of Stoermer-Verlet.
     */
    class StoermerVerlet final : public IntegratorBase {
       public:
        using IntegratorBase::IntegratorBase;

       private:
       
         /**
         * @brief Performs a single step of the Stoermer-Verlet.
         * @param step
         * @param dt Δt The time increment for each simulation step.
         */
        void simulation_step(unsigned step, double dt) override;

        /**
         * @brief TODO
         * @param step
         * @param dt Δt The time increment for each simulation step.
         */
        void simulation_step_omp1(unsigned step, double dt) override;
    };

}  // namespace md::Integrator
