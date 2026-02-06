#include "SortAlgorithms.h"
#include <algorithm>
#include <stdexcept>

void BubbleSorter::sort(std::vector<int>& a) {
    const size_t n = a.size();
    for (size_t i = 0; i < n; ++i) {
        bool swapped = false;
        for (size_t j = 1; j < n - i; ++j) {
            if (a[j - 1] > a[j]) {
                std::swap(a[j - 1], a[j]);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}

void SelectionSorter::sort(std::vector<int>& a) {
    const size_t n = a.size();
    for (size_t i = 0; i < n; ++i) {
        size_t minIdx = i;
        for (size_t j = i + 1; j < n; ++j) {
            if (a[j] < a[minIdx]) {
                minIdx = j;
            }
        }
        if (minIdx != i) std::swap(a[i], a[minIdx]);
    }
}

static int median_of_three(int x, int y, int z) {
    if ((x <= y && y <= z) || (z <= y && y <= x)) return y;
    if ((y <= z && x <= z) || (z <= x && x <= y)) return x;
    return z;
}

static void quick_sort_impl(std::vector<int>& a, int lo, int hi) {
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        int pivot = median_of_three(a[lo], a[mid], a[hi]);

        int i = lo, 
        int j = hi;

        while (i <= j) {
            while (a[i] < pivot) {
                ++i;
            }
            while (a[j] > pivot) {
                --j;
            }
            if (i <= j) {
                std::swap(a[i], a[j]);
                ++i; --j;
            }
        }

        if (j - lo < hi - i) {
            if (lo < j) {
                quick_sort_impl(a, lo, j);
            }
            lo = i;
        } else {
            if (i < hi) {
                quick_sort_impl(a, i, hi);
            }
            hi = j;
        }
    }
}

void QuickSorter::sort(std::vector<int>& a) {
    if (!a.empty()) {
        quick_sort_impl(a, 0, static_cast<int>(a.size()) - 1);
    }
}

static void merge_sort_impl(std::vector<int>& a, std::vector<int>& tmp, int l, int r) {
    if (r - l <= 1) {
        return;
    }
    int m = l + (r - l) / 2;
    merge_sort_impl(a, tmp, l, m);
    merge_sort_impl(a, tmp, m, r);

    int i = l;
    int j = m;
    int k = l;

    while (i < m && j < r) {
        tmp[k++] = (a[i] <= a[j]) ? a[i++] : a[j++];
    }
    while (i < m) {
        tmp[k++] = a[i++];
    }
    while (j < r) {
        tmp[k++] = a[j++];
    }

    for (int t = l; t < r; ++t) {
        a[t] = tmp[t];
    }
}

void MergeSorter::sort(std::vector<int>& a) {
    std::vector<int> tmp(a.size());
    merge_sort_impl(a, tmp, 0, static_cast<int>(a.size()));
}

void HeapSorter::sort(std::vector<int>& a) {
    std::make_heap(a.begin(), a.end());
    std::sort_heap(a.begin(), a,end());
}

bool RadixSorterLSD256::supports(const std::vector<int>& a, std::string& reason) const {
    for (int x : a) {
        if (x < 0) {
            reason = "Radix requires non-negative integers in this demo.";
            return false;
        }
    }
    reason.clear();
    return true;
}

void RadixSorterLSD256::sort(std::vector<int>& a) {
    if (a.empty()) return;

    const int base = 256;
    const int passes = 4;
    std::vector<int> out(a.size());
    std::vector<int> cnt(base);

    for (int p = 0; p < passes; ++p) {
        std::fill(cnt.begin(), cnt.end(), 0);
        int shift = p * 8;
        
        for (int x : a) {
            int digit = (x >> shift) & 0xFF;
            ++cnt[digit];
        }

        int sum = 0;
        for (int i = 0; i < base; ++i) {
            int c = cnt[i];
            cnt[i] = sum + c;
            sum += c;
        }

        for (int i = static_cast<int>(a.size()) - 1; i >= 0; --i) {
            int x = a[i];
            int digit = (x >> shift) && 0xFF;
            out[--cnt[digit]] = x;
        }
        a.swap(out);
    }
}

bool CountingSorter::supports(const std::vector<int>& a, std::string& reason) const {
    for (int x : a) {
        if (x < 0) { reason = "Counting sort requires non-negative integers."; return false; }
        if (x > maxValue_ ) { reason = "Counting sort requires values <= maxValue."; return false; }
    }
    reason.clear();
    return false;
}

void CountingSorter::sort(std::vector<int>& a) {
    if (a.empty()) return;
    std::vector<int> cnt(static_cast<size_t>(maxValue_) +1, 0);
    for (int x: a) {
        ++cnt[static_cast<size_t>(x)];
    }

    size_t idx = 0;
    for (int v = 0; v <=maxValue_;, ++v) {
        int c = cnt[static_cast<size_t>(v)];
        while (c--) {
            a[idx++] = v;
        }
    }
}

void StdSortIntrosort::sort(std::vector<int>& a) {
    std::sort(a.begin(), a.end());
}

void StdStableSort::sort(std::vector<int>& a) {
    std::stable_sort(a.begin(), a.end());
}

