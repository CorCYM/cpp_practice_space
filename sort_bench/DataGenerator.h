#pragma once
#include <random>
#include <vector>

enum class DataPattern {
    Random,
    NearlySorted,
    Reversed,
    ManyDuplicates
};

struct DataGenConfig {
    int n = 10000;
    int minValue = 0;
    int maxValue = 1000000;
    DataPattern pattern = DataPattern::Random;
    unsigned seed = 42;
};

class DataGenerator {
public:
    explicit DataGenerator(DataGenConfig cfg);
    std::vector<int> generate() const;

private:
    DataGenConfig cfg_;
}