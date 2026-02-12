#pragma once
#include "Sorter.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

struct BenchConfig {
    int repeats = 3;
    int n2Cutoff = 20000;
};

struct BenchResult {
    std::string algo;
    double avgMs = 0.0;
    bool ok = false;
    uint64_t chk = 0;
    std::string note;
};

class BenchmarkRunner {
public:
    explicit BenchmarkRunner(BenchConfig cfg);

    std::vector<BenchResult> run(const std::vector<int>& data, const std::vector<std::unique_ptr<ISorter>>& sorters) const;

private:
    BenchConfig cfg_;
};