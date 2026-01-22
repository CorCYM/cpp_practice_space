#pragma once

#include <string>
#include <array>
#include <vector>

struct Question {
    std::string text;
    std::array<std::string, 4> options{};
    char correctOption{'A'};
    int timeoutSeconds{30};
    int prizeMoney{0};
};

std::vector<Question> loadQuestions(const std::string& fileName);