#include "SortAlgorithms.h"
#include "DataGenerator.h"
#include "BenchmarkRunner.h"
#include "Report.h"

int main() {
    auto sorters = make_default_sorters();

    BenchConfig bc;
    bc.repeats = 3;
     bc.n2Cutoff = 20000;

     BenchmarkRunner runner(bc);

     std::vector<int> sizes = {1000, 5000, 20000, 100000};

     for (int N : sizes) {
        DataGenConfig dg;
        dg.n = N;
        dg.minValue = 0;
        dg.maxValue = 1000000;
        dg.pattern = DataPattern::Random;
        dg.seed = 42;

        DataGenerator gen(dg);
        auto data = gen.generate();

        auto results = runner.run(data, sorters);
        Report::print(N, bc.repeats, results);

    }

    return 0;
}