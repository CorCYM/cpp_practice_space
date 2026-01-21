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
    if (!file.is_open()) {
        std::cerr << "Unable to open question file: " << fileName << '\n';
        std::exit(1);
    }

    std::vector<Question> questions;
    std::string line;

    while (true) {
        Question q;

        if (!std::getline(file, line)) {
            break;
        }
        q.text = trim(line);
        if (q.text.empty()) {
            continue;
        }
        
        bool ok = true;
        for (int i = 0; i < 4; ++i) {
            if (!std::getline(file, line)) {
                ok = false;
                break;
            }
            q.options[i] = trim(line);
        }
        if (!ok) {
            break;
        }

        if (!std::getline(file, line)) break;
        line = trim(line);
        if (line.empty()) break;
        q.correctOption = static_cast<char>(std::toupper(static_cast<unsigned char>(line[0])));
        
        if (!std::getline(file, line)) break;
        line = trim(line);
        try {
            q.timeoutSeconds = std::stoi(line);
        } catch (...) {
            q.timeoutSeconds = 30;
        }

        if (!std::getline(file, line)) break;
        line = trim(line);
        try {
            q.prizeMoney = std::stoi(line);
        } catch (...) {
            q.prizeMoney = 0;
        }

        questions.push_back(q);

        std::streampos pos = file.tellg();
        if (!std::getline(file, line)) {
            break;
        }
        if (!trim(line).empty()) {
            file.seekg(pos);
        }
    }

    return questions;
}