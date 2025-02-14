#pragma once

#include "core/IntegratorBase.h"

namespace md::Integrator {

    /**
     * @brief Implements the calculation of Stoermer-Verlet using cell lock as parallelization strategy.
     */
    class StoermerVerletCellLock final : public IntegratorBase {
    public:
        using IntegratorBase::IntegratorBase;

    private:

        /**
        * @brief Performs a single step of the Stoermer-Verlet, with cell locking.
        * @param step
        * @param dt Δt The time increment for each simulation step.
        */
        void simulation_step(unsigned step, double dt) override;
    };

}  // namespace md::Integrator