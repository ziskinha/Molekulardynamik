#pragma once
#include "env/Environment.h"

namespace md::core {
    class Statistics {
    public:
        explicit Statistics(const int compute_freq = 10000): compute_freq(compute_freq) {}
        virtual ~Statistics() = default;
        virtual void compute(const env::Environment & env) = 0;
        const int compute_freq;
    };


    class NanoFlowStatistics final : public Statistics {
    public:
        NanoFlowStatistics(int compute_freq, int n_bins);
        void compute(const env::Environment & env) override;
    private:
        int n_bins;
    };
}



