#include "Report.h"
#include <iomanip>
#include <iostream>

void Report::print(int N, int repeats, const std::vector<BenchResult>& results) {
    std::cout << "\n====================================================\n";
    std::cout << "N = " << N << ", repeats = " << repeats << "\n";
    std::cout << std::left << std::setw(22) << "Algorithm" << std::right << std::setw(12) << "Avg(ms)"
              << std::setw(8) << "OK" << "  " << "Note/Checksum\n";
    std::cout << "----------------------------------------------------\n";

    for (const auto& r : results) {
        std::cout << std::left << std::setw(22) << r.algo;

        if (!r.note.empty()) {
            std::cout << std::right << std::setw(12) << "-" << std::setw(8) << (r.ok ? "YES" : "NO") << "   " << r.note << "\n";
        } else {
            std::cout << std::right << std::setw(12) << std::fixed << std::setprecision(3) << r.avgMs 
                      << std::setw(8) << (r.ok ? "YES" : "NO") << "  " << r.chk << "\n";
        }
    }
    std::cout << "====================================================\n";
}