#pragma once
#include "env/Environment.h"
#include "io/Output/CSVWriter.h"

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
        virtual void compute(const env::Environment & env, double time) = 0;
        virtual ~Statistics() {}

        /**
         * @brief number of iterations between each statistics calculation.
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
         * @brief Computes the nano-scale flow statistics, such as for velocity and density and writes them to a CSV file.
         * @param env
         */
        void compute(const env::Environment & env, double time) override;
    private:
        int n_bins;                        ///< Number of bins.
        md::io::CSVWriter writer;          ///< A CSV Writer  for the statistics.
    };
} // namespace md::core



