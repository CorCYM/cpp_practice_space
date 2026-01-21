#include "QuizGame.h"

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <memory>
#include <cctype>
#include <cstdlib>

namespace {
    constexpr const char* COLOR_END = "\033[0m";
    constexpr const char* RED       = "\033[1;31m";
    constexpr const char* GREEN     = "\033[1;32m";
    constexpr const char* YELLOW    = "\033[1;33m";
    constexpr const char* BLUE      = "\033[1;34m";
    constexpr const char* PINK      = "\033[1;35m";
    constexpr const char* AQUA      = "\033[1;36m";

    struct Inputstate {
        std::mutex mtx;
        std::condition_variable cv;
        bool answered{false};
        std::string answer;
    };

    void inputThreadFunc(std::shared_ptr<InputState> state) {
        std::string line;
        if (std::getline(std::cin, line)) {
            std::unique_lock<std::mutex> lock(state->mtx);
            state->answer = line;
            state->answered = true;
            lock.unlock();
            state->cv.notify_one();
        }
    }
}

QuizGame::QuizGame(std::vector<Question> questions) : questions_(std::move(questions)) {
    lifelineAvailable_[0] = 1;
    lifelineAvailable_[1] = 1;
}

void QuizGame::printIntro() const {
    std::cout << "\n\n" << PINK << "\t\tLet's play Who Wants To Be A Millionaire!!!" << COLOR_END << "\n";
}

void QuizGame::printOutro() const {
    std::cout << "\n" << BLUE << "Your total winnings are: $ " << moneyWon_ << COLOR_END << "\n";
}

void QuizGame::printQuestion(const Question& q) const {
    std::cout << "\n\n" << YELLOW << q.text << COLOR_END << " \n";

    for (int i = 0; i < 4; ++i) {
        if (!q.options[i].empty()) {
            std::cout << AQUA << static_cast<char>('A' + i) << ". " << q.options[i] << COLOR_END << "\n";
        }
    }

    std::cout << YELLOW << "Hurry!! You have only " << q.timeoutSeconds << " seconds to answer.." << COLOR_END << "\n";

    std::cout << GREEN << "Enter your answer (A, B, C, or D) or L for lifeline: " << COLOR_END << std::flush;
}

char QuizGame::readAnswerWithTimeout(const Question& q) const {
    auto state = std::make_shared<InputState>();
    std::thread inputThread(inputThreadFunc, state);

    char result = 0;

    {
        std::unique_lock<std::mutex> lock(state->mtx);
        auto timeout = std::chrono::seconds(q.timeoutSeconds);
        auto deadline = std::chrono::steady_clock::now() + timeout;

        while (!state->answered) {
            if (state->cv.wait_until(lock, deadline) == std::cv_status::timeout) {
                break;
            }
        }

        if (state->answered && !state->answer.empty()) {
            char ch = static_cast<char>(std::toupper(static_cast<unsigned char>(state->answer[0])));
            if (ch == 'A' || ch == 'B' || ch == 'C' || ch == 'D' || ch == 'L') {
                result = ch;
            }
        }
    }

    if (result == 0) {
        std::cout << "\n" << RED << "Time out!!!!!" << COLOR_END << "\n";
    }

    inputThread.detach();

    return result;
}

QuizGame::LifelineResult QuizGame::useLifeline(Question& q) {
    std::cout << "\n\n" << PINK << "Available Lifelines:" << COLOR_END;
    if (lifelineAvailable_[0]) {
        std::cout << "\n" << PINK << "1. Fifty-Fifty (50/50)" << COLOR_END;
    }
    if (lifelineAvailable_[1]) {
        std::cout << "\n" << PINK << "2. Skip the Question" << COLOR_END;
    }
    std::cout << "\n" << PINK << "Choose a lifeline or 0 to return: " << COLOR_END << std::flush;

    std::string line;
    if (!std::getline(std::cin, line)) {
        return LifelineResult::None;
    }
    if (line.empty()) {
        return LifelineResult::None;
    }
    char ch = line[0];

    switch (ch) {
        case '1':
            if (lifelineAvailable_[0]) {
                lifelineAvailable_[0] = 0;

                int removed = 0;
                while (removed < 2) {
                    int idx = std::rand() % 4;
                    char label = static_cast<char>('A' + idx);
                    if (label != q.correctOption && !q.options[idx].empty()) {
                        q.options[idx].clear();
                        ++removed;
                    }
                }

                std::cout << "\n" << PINK << "50/50 lifeline used." << COLOR_END << "\n";
                return LifelineResult::FiftyFiftyUsed;
            } else {
                std::cout << "\n" << PINK << "50/50 already used." << COLOR_END << "\n";
            }
            break;

        case '2':
            if (lifelineAvailable_[1]) {
                lifelineAvailable_[1] = 0;
                std::cout << "\n" << PINK << "Skip lifeline used. Skipping question." << COLOR_END << "\n";
                return LifelineResult::SkipQuestion;
            } else {
                std::cout << "\n" << PINK << "Skip already used." << COLOR_END << "\n";
            }
            break;
        
        default:
            std::cout << "\n" << PINK << "Returning to the question." << COLOR_END << "\n";
            break;
    }

    return LifelineResult::None;
}

void QuizGame::run() {
    printIntro();

    for (Question& q : questions_) {
        bool skipThisQuestion = false;

        while (true) {
            printQuestion(q);
            char answer = readAnswerWithTimeout(q);

            if (answer == 0) {
                std::cout << "\n" << RED << "You failed to answer in time." << COLOR_END << "\n";
                printOutro();
                return;
            }

            if (answer == 'L') {
                LifelineResult lr = useLifeline(q);
                if (lr == LifelineResult::SkipQuestion) {
                    skipThisQuestion = true;
                    break;
                }
                continue;
            }

            if (answer == q.correctOption) {
                std::cout << "\n" << GREEN << "Correct!" << COLOR_END << "\n";
                moneyWon_ = q.prizeMoney;
                std::cout << BLUE << "You have won: $ " << moneyWon_ << COLOR_END << "\n";
            } else {
                std::cout << "\n" << RED << "Wrong! Correct answer is " << q.correctOption << "." << COLOR_END << "\n";
                printOutro();
                return;
            }
            break;
        }

        if (skipThisQuestion) {
            continue;
        }
    }

    std::cout << "\n" << BLUE << "Congratulations! You finished all questions.\n" << "Your total winnings are: $ " << moneyWon_ << COLOR_END << "\n";
}
