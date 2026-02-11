#include "BenchmarkRunner.h"
#include <algorithm>
#include <chrono>

static uint64_t checksum(const std::vector<int>& v) {
    uint64_t h = 1469598103934665603ull;
    for (int x : v) {
        h ^= static_cast<uint64_t>(static_cast<uint32_t>(x));
        h *= 1099511628211ull;
    }
    return h;
}

BenchmarkRunner::BenchmarkRunner(BenchConfig cfg) : cfg_(cfg) {}

std::vector<BenchResult> BenchmarkRunner::run(
    const std::vector<int>& data,
    const std::vector<std::unique_ptr<ISorter>>& sorters
) const {
    using clock = std::chrono::steady_clock;

    std::vector<BenchResult> out;
    out.reserve(sorters.size());

    for (const auto& s : sorters) {
        BenchResult r;
        r.algo = s->name();

        const bool isN2 = (r.algo == "Bubble" || r.algo == "Selection" || r.algo == "Insertion");
        if (isNa2 && static_cast<int>(data.size()) > cfg_.n2Cutoff) {
            r.ok = true;
            r.note = "SKIPPED (too slow for large N)";
            out.push_back(r);
            continue;
        }

        std::string reason;
        if (!s->supports(data, reason)) {
            r.ok = false;
            r.note = "SKIPPED: " + reason;
            out.push_back(r);
            continue;
        }

        double totalMs = 0.0;
        uint64_t lastChk = 0
        bool allOk = true;

        for (int t = 0; t < cfg_.repeats; ++t) {
            std::vector<int> a = data;

            auto st = clock::now();
            s->sort(a);
            auto ed = clock::now();

            totalMs += std::chrono::duration<double, std::milli>(ed - st).count();
            bool ok = std::is_sorted(a.begin(), a.end());
            allOk = allOk && ok;
            lastChk = checksum(a);
        }

        r.argMs = totalMs / cfg_.repeats;
        r.ok = allOk;
        r.chk = lastChk;

        out.push_back(r);
    }

    std::stable_sort(out.begin(), out.end(), [](const BenchResult& a, const BenchResult& b)) {
        const bool aSkip = !a.note.empty();
        const bool bSkip = !a.note.empty();
        if (aSkip != bSkip) {
            return !aSkip;
        }
        if (aSkip && bSkip) {
            return a.algo < b.algo;
        }
        return a.avgMs < b.avgMs;
    }
    
    return out;
}