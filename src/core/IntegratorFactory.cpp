#include "IntegratorFactory.h"
#include "StoermerVerlet/StoermerVerlet.h"
#include "StoermerVerlet/StoermerVerletCellLock.h"
#include "StoermerVerlet/StoermerVerletSpatialDecomp.h"
#include "io/Logger/Logger.h"

namespace md::Integrator {
    std::unique_ptr<IntegratorBase> create_simulator(io::ProgramArguments &args, std::unique_ptr<core::Statistics> stats) {

        auto writer = create_writer(args.output_baseName, args.output_format, args.override);
        auto checkpoint_writer = io::create_checkpoint_writer();

#ifndef _OPENMP
        if (args.parallel_strategy == 1 || args.parallel_strategy == 2) {
            SPDLOG_INFO("A parallelization strategy was selected, but the program was compiled without the OpenMP flag. "
                        "To enable parallelization, please compile with the \"-fopenmp\" flag.");
            exit(-1);
        }
#endif

        switch (args.parallel_strategy) {
            case 1:
                return std::make_unique<StoermerVerletCellLock>(args.env, std::move(writer), std::move(checkpoint_writer),
                                                                args.thermostat, args.external_forces, std::move(stats));
            case 2:
                return std::make_unique<StoermerVerletSpatialDecomp>(args.env, std::move(writer), std::move(checkpoint_writer),
                                                                     args.thermostat, args.external_forces, std::move(stats));
            default:
                return std::make_unique<StoermerVerlet>(args.env, std::move(writer), std::move(checkpoint_writer),
                                                        args.thermostat, args.external_forces, std::move(stats));
        }

    }
} // namespace md::Integrator
