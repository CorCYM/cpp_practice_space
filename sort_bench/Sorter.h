#pragma once
#include <string>
#include <vector>

class ISorter {
public:
    virtual ~ISorter() = default;
    virtual std::string name() const = 0;
    virtual void sort(std::vector<int>& a) = 0;

    virtual bool supports(const std::vector<int>& a, std::string& reason) const {
        (void) a;
        reason.clear();

        return true;
    }
};