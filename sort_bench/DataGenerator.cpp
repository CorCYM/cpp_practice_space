#include "DataGenerator.h"
#include <algorithm>

DataGenerator::DataGenerator(DataGenConfig cfg) : cfg_(cfg) {}

std::vector<int> DataGenerator::generate() const {
    std::mt19937 rng(cfg_.seed);
    std::uniform_int_distribution<int> dist(cfg_.minValue, cfg_.maxValue);

    std::vector<int> a(cfg_.n);
    for (int i = 0; i < cfg_.n; ++i) {
        a[i] = dist(rng);
    }

    switch (cfg_.pattern) {
        case DataPattern::Random:
            break;
        case DataPattern::NearlySorted:
            std::sort(a.begin(), a.end());
            for (int k = 0; k < cfg_.n / 100; ++k) {
                int i = dist(rng) % cfg_.n;
                int j = dist(rng) % cfg_.n;
                std::swap(a[i], a[j]);
            }
            break;
        case DataPattern::Reversed:
            std::sort(a.begin(), a.end());
            std::reverse(a.begin(), a.end());
            break;
        case DataPattern::ManyDuplicates:
            {
                std::uniform_int_distribution<int> small(cfg_.minValue, cfg_.minValue + 1000);
                for (int i = 0; i < cfg_.n; ++i) {
                    a[i] = small(rng);
                }
            }
            break;
    }
    return a;
}