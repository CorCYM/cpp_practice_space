#pragma once
#include "Sorter.h"
#include <memory>
#include <vector>

class BubbleSorter final : public ISorter {
public:
    std::string name() const override { return "Bubble"; }
    void sort(std::vector<int>& a) override;
};

class SelectionSorter final : public ISorter {
public:
    std::string name() const override { return "Selection"; }
    void sort(std::vector<int>& a) override;
};

class InsertionSorter final :public ISorter {
public:
    std::string name() const override { return "Insertion"; }
    void sort(std::vector<int>& a) override;
};

class QuickSorter final : public ISorter {
public:
    std::string name() const override { return "Quick"; }
    void sort(std::vector<int>& a) override;
};

class MergeSorter final : public ISorter {
public:
    std::string name() const override { return "Merge"; }
    void sort(std::vector<int>& a) override;
};

class HeapSorter final : public ISorter {
public:
    std::string name() const override { return "Heap"; }
    void sort(std::vector<int>& a) override;
};

class RadixSorterLSD256 final : public ISorter {
public:
    std::string name() const override { return "Radix"; }
    bool support(const std::vector<int>& a, std::string& reason) const override;
    void sort(std::vector<int&> a) override;
};

class CountingSorter final : public ISorter {
public:
    explicit CountingSorter(int maxValueInclusive) : maxValue_(maxValueInclusive) {}
    std::string name() const override { return "Counting"; }
    bool supports(const std::vector<int>& a, std::string& reason) const override;
    void sort(std::vector<int>& a) override;

private:
    int maxValue_;
};

class StdSortIntrosort final : public ISorter {
public:
    std::string name() const override { return "std::sort(Introsort)"; }
    void sort(std::vector<int>& a) override;
};


class StdStableSort final : public ISorter {
public:
    std::string name() const override { return "std::stable_sort" }
    void sort(std::vector<int>& a) override;
};

class ShellSorter final : public ISorter {
public:
    std::string name() const override { return "Shell"; }
    void sort(std::vector<int>& a) override;
};

std::vector<std::unique_ptr<ISorter>> make_default_sorters();