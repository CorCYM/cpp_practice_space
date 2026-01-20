#include "Question.h"

#include <fstream>
#include <iostream>
#include <cctype>
#include <cstdlib>

namespace {
    std::string trim(const std::string& s) {
        std::size_t start = 0;
        while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
            ++start;
        }
        if (start == s.size()) {
            return std::string{};
        }

        std::size_t end = s.size();
        while (end > start && std::isspace(static_cast<unsigned char>(s[end -1]))) {
            --end;
        }

        return s.substr(start, end - start);
    }
}

std::vector<Question> loadQuestions(const std::string& fileName) {
    std::ifstream file(fileName);
    

}