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
         * @param dt Δt The time increment for each simulation step.
         */
        void simulation_step(double dt) override;

        // TODO is ugly -> remove
        // Friend function declaration, granting access to private members
        friend void dummy_run_simulation(StoermerVerlet& obj, double dt);
    };

    // TODO is ugly -> remove
    void dummy_run_simulation(StoermerVerlet& obj, double dt);

}  // namespace md::Integrator
