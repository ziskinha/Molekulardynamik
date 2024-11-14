#pragma once

#include "IntegratorBase.h"
#include "force.h"

namespace md::Integrator {

    /**
     * @brief Implements the calculation of Stoermer-Verlet.
     */
    class StoermerVerlet final : public IntegratorBase {
       public:
        /**
         * @brief Constructs a StoermerVerlet object with specified force calculation.
         * @param particles contains the to-be-simulated particles
         * @param force_func Defines the force function which is being used.
         * @param writer writes the output to disk
         */
        StoermerVerlet(ParticleContainer& particles, force::ForceFunc force_func,
                       std::unique_ptr<io::OutputWriterBase> writer);

        // Friend function declaration, granting access to private members
        friend void dummy_run_simulation(StoermerVerlet& obj, double dt);

       private:
        /**
         * @brief Performs a single step of the Stoermer-Verlet.
         * @param dt Δt The time increment for each simulation step.
         */
        void simulation_step(double dt) override;
        force::ForceFunc force_func;
    };

    void dummy_run_simulation(StoermerVerlet& obj, double dt);

}  // namespace md::Integrator
