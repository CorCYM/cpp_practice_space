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
    if (lifeline_used[0]) {
        std::cout << "\n" << PINK << "1. Fifty-Fifty (50/50)" << COLOR_END;
    }
    if (lifeline_used[1]) {
        std::cout << "\n" << PINK << "2. Skip the Question" << COLOR_END;
    }
    std::cout << "\n" << PINK << "Choose a lifeline or 0 to return: " << COLOR_END << flush;

    std::string line;
    if (!std::getline(cin, line)) {
        return 0;
    }
    if (line.empty()) return 0;
    char ch = line[0];

    switch (ch) {
        case '1':
            if (lifeline_used[0]) {
                lifeline_used[0] = 0;
                {
                    int removed = 0;
                    while (removed < 2) {
                        int idx = rand() % 4;
                        char opt_label = static_cast<char>('A' + idx);
                        if (opt_label != q.correct_option && !q.options[idx].empty()) {
                            q.options[idx].clear();
                            ++removed;
                        }
                    }
                }
                std::cout << "\n" << PINK << "50/50 lifeline used." << COLOR_END << "\n";
                return 1;
            } else {
                std::cout << "\n" << PINK << "50/50 already used." << COLOR_END << "\n";
            }
            break;
        case '2':
            if (lifeline_used[1]) {
                lifeline_used[1] = 0;
                std::cout << "\n" << PINK << "Skip lifeline used. Skipping question." << COLOR_END << "\n";
                return 2;
            } else {
                std::cout << "\n" << PINK << "Skip already used." << COLOR_END << "\n";
            }
            break;
        default:
            std::cout << "\n" << PINK << "Returning to the question." << COLOR_END << "\n";
            break;
    }
    return 0;
}

char read_answer_with_timeout(const Question &q) {
    mutex mtx;
    condition_variable cv;
    bool answered = false;
    std::string answer;

    std::thread input_thread([&]() {
        std::string line;
        if (std::getline(cin, line)){
            {
                unique_lock<mutex> lock(mtx);
                answer = line;
                answered = true;
            }
            cv.notify_one();
        }
    });

    char result = 0;

    {
        unique_lock<mutex> lock(mtx);
        if (cv.wait_for(lock, std::chrono::seconds(q.timeout_seconds), [&] { return answered; })) {
            if (!answer.empty()) {
                char ch = static_cast<char>(toupper(
                    static_cast<unsigned char>(answer[0])));
                if (ch == 'A' || ch == 'B' || ch == 'C' || ch == 'D' || ch == 'L') {
                    result = ch;
                }
            }
        } else {
            std::cout << "\n" << RED << "\nTime out!!!!!" << COLOR_END << "\n";
            result = 0;
        }
    }

    input_thread.detach();

    return result;
}

void play_game(std::vector<Question> questions) {
    int money_won = 0;
    int lifeline[2] = {1, 1};

    std::cout << "\n\n" << PINK << "\t\tLet's play Who Wants To Be A Millionaire!!!" << COLOR_END << "\n";

    for (size_t i = 0; i < questions.size(); ++i) {
        Question &q = questions[i];

        while (true) {
            print_formatted_question(q);
            char ch = read_answer_with_timeout(q);

            if (ch == 0) {
                std::cout << "\n" << RED << "You failed to answer in time." << COLOR_END << "\n";
                std::cout << "\n" << BLUE << "Game Over! Your total winnings are: $ " << money_won << COLOR_END << "\n";
                return;
            }

            if (ch == 'L') {
                int val == use_lifeline(q, lifeline);
                if (val == 2) {
                    goto next_question;
                }
                continue;
            }

            if (ch == q.correct_option) {
                std:cout << "\n" << GREEN << "Correct!" << COLOR_END << "\n";
                money_won = q.prize_money;
                std::cout << BLUE << "You have won: $ " << money_won << COLOR_END << "\n";
            } else {
                std::cout << "\n" << RED << "Wrong! Correct answer is " << q.correct_option << "." << COLOR_END << "\n";
                std::cout << "\n" << BLUE << "Game Over! Your total winnings are: $ " << money_won << COLOR_END << "\n";
                return;
            }

            break;
        }

    next_question:
        continue;
    }

    std::cout << "\n" << BLUE << "Congratulations! You finished all questions.\n" << "Your total winnings are: $ " << money_won << COLOR_END << "\n";
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    std::vector<Question> questions = read_questions("questions.txt");
    if (questions.empty()) {
        std::cerr << "No questions loaded. Please check questions.txt format. \n";
        return 1;
    }

    play_game(questions);

    return 0;
}