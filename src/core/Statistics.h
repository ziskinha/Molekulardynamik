#pragma once
#include "env/Environment.h"

namespace md::core {

    /**
     * @brief Class fot statistical analysis of the simulation.
     */
    class Statistics {
    public:
        explicit Statistics(const int compute_freq = 10000): compute_freq(compute_freq) {}

        /**
         * @brief virtual method fot computing statistics.
         * @param env
         */
        virtual void compute(const env::Environment & env) = 0;
        virtual ~Statistics() {}

        /**
         * @brief number of iterations between each statistics calculation
         */
        const int compute_freq;
    };


    /**
     * @brief Class for statistical analysis of the nano-scale flow.
     */
    class NanoFlowStatistics final : public Statistics {
    public:
        NanoFlowStatistics(int compute_freq, int n_bins);

        /**
         * @brief Computes the nano-scale flow statistics, such as for velocity and density and TODO writes them to a CSV file.
         * @param env
         */
        void compute(const env::Environment & env) override;
    private:
        int n_bins;  ///< The number of bins along the x-axis.
    };
} // namespace md::core



