#pragma once
#include "BenchmarkRunner.h"
#include <vector>

class Report {
public:
    static void print(int N, int repreats, const std::vector<BenchResult>& results);
};