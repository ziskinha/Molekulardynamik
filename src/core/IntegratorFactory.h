#pragma once

#include "IntegratorBase.h"
#include "utils/Parse.h"

namespace md::Integrator {
    /**
     * @brief Creates a simulator object based on the chosen parallelization strategy.
     * @param args Program arguments for configuring the simulation.
     * @param stats (optional)
     * @return A unique pointer to an "IntegratorBase" object.
     */
    std::unique_ptr<IntegratorBase> create_simulator(io::ProgramArguments &args,
                                                     std::unique_ptr<core::Statistics> stats = nullptr);
}
