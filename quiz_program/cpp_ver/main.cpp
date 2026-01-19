#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <cctype>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

using namespace std;

const char *COLOR_END = "\033[0m";
const char *RED       = "\033[1;31m";
const char *GREEN     = "\033[1;32m";
const char *YELLOW    = "\033[1;33m";
const char *BLUE      = "\033[1;34m";
const char *PINK      = "\033[1;35m";
const char *AQUA      = "\033[1;36m";

struct Question {
    std::string text;
    std::array<string, 4> options;
    char correct_option = 'A';
    int timeout_seconds = 30;
    int prize_money = 0;
};

std::string trim(const std::string &s) {
    size_t start = 0;
    while (start < s.size() && isspace(static_cast<unsigned char>(s[start]))) {
        ++start;
    }
    if (start == s.size()) return "";

    size_t end = s.size() - 1;
    while (end > start && isspace(static_cast<unsigned char>(s[end]))) {
        --end;
    }
    return s.substr(start, end - start + 1);
}

std::vector<Question> read_questions(const std::string &file_name) {
    ifstream file(file_name);
    if (!file.is_open()) {
        std::cerr << "Unable to open question file: " << file_name << std::endl;
        exit(1);
    }

    std::vector<Question> questions;
    std::string line;

    while (true) {
        Question q;

        if (!std::getline(file, line)) break;
        q.text = trim(line);
        if (q.text.empty()) {
            continue;
        }

        bool ok = true;
        for (int i = 0; i < 4; i++) {
            if (!std::getline(file, line)) {
                ok = false;
                break;
            }
            q.options[i] = trim(line);
        }
        if (!ok) break;

        if (!std::getline(file, line)) break;
        line = trim(line);
        if (line.empty()) break;
        q.correct_option = static_cast<char>(toupper(static_cast<unsigned char>(line[0])));

        if (!std::getline(file, line)) break;
        line = trim(line);
        try {
            q.timeout_second = stoi(line);
        } catch (...) {
            q.timeout_seconds = 30;
        }

        if (!std::getline(file, line)) break;
        line = trim(line);
        try {
            q.prize_money = stoi(line);
        } catch (...) {
            q.prize_money = 0;
        }

        questions.push_back(q);

        std::streampos pos = file.tellg();
        if (std::getline(file, line)) {
            if (!trim(line).empty()) {
                file.seekg(pos);
            }
        } else {
            break;
        }
    }

    return questions;
}


void print_formatted_question(const Question &q) {
    std::cout << "\n\n" << YELLOW << q.text << COLOR_END << "\n";

    for (int i = 0; i < 4; ++i) {
        if (!q.options[i].empty()) {
            std::cout << AQUA << static_cast<char>('A' + i) << ". " << q.options[i] << COLOR_END << "\n";
        }
    }

    std::cout << YELLOW << "Hurry!! You have only " << q.timeout_seconds << " seconds to answer.." << COLOR_END << "\n";

    std::cout << GREEN << << "Enter your answer (A, B, C, or D) or L for lifeline: " << COLOR_END << std::flush;
}

int use_lifeline(Question &q, int *lifeline_used) {
    std::cout << "\n\n" << PINK << "Available Lifelines:" << COLOR_END;
    if () {
        
    }

}