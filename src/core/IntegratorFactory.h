#pragma once

#include "IntegratorBase.h"
#include "utils/Parse.h"

namespace md::Integrator {
    /**
     * @brief Creates and returns a simulator object based on the chosen parallelization strategy.
     * @param args Program arguments
     * @param stats (optional)
     * @return
     */
    std::unique_ptr<IntegratorBase> create_simulator(io::ProgramArguments &args,
                                                     std::unique_ptr<core::Statistics> stats = nullptr);
}
