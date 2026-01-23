#include "QuizGame.h"

#include <iostream>
#include <chrono>
#include <cctype>
#include <cstdlib>


constexpr const char* COLOR_END = "\033[0m";
constexpr const char* RED       = "\033[1;31m";
constexpr const char* GREEN     = "\033[1;32m";
constexpr const char* YELLOW    = "\033[1;33m";
constexpr const char* BLUE      = "\033[1;34m";
constexpr const char* PINK      = "\033[1;35m";
constexpr const char* AQUA      = "\033[1;36m";

/*
struct QuizGame::InputState {
    std::mutex mtx;
    std::condition_variable cv;
    bool answered{false};
    std::string answer;
};

void QuizGame::inputThreadFunc(std::shared_ptr<InputState> state) {
    std::string line;
    if (std::getline(std::cin, line)) {
        std::unique_lock<std::mutex> lock(state->mtx);
        state->answer = line;
        state->answered = true;
        lock.unlock();
        state->cv.notify_one();
    }
}
*/

QuizGame::QuizGame(std::vector<Question> questions) : questions_(std::move(questions)) {
    lifelineAvailable_[0] = 1;
    lifelineAvailable_[1] = 1;
}

QuizGame::~QuizGame(){
    stopInputThread();
}

void QuizGame::startInputThread() {
    stop_ = false;
    inputThread_ = std::thread(&QuizGame::inputLoop, this);
}

void QuizGame::stopInputThread() {
    if (!inputThread_.joinable()) return;

    stop_ = true;

    inputCv_.notify_all();

    inputThread_.join();
}

void QuizGame::inputLoop() {
    std::string line;
    while (!stop_) {
        if (!std::getline(std::cin, line)) {
            break;
        }

        {
            std::lock_guard<std::mutex> lock(inputMtx_);
            inputQueue_.push(line);
        }
        inputCv_.notify_one();
    }

    inputCv_.notify_all();
}

bool QuizGame::popLineWithTimeout(int timeoutSeconds, std::string& outLine) {
    std::unique_lock<std::mutex> lock(inputMtx_);

    if (!inputQueue_.empty()) {
        outLine = std::move(inputQueue_.front());
        inputQueue_.pop();
        return true;
    }

    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(timeoutSeconds);

    while (inputQueue_.empty() && !stop_) {
        if (inputCv_.wait_until(lock, deadline) == std::cv_status::timeout) {
            break;
        }
    }

    if (!inputQueue_.empty()) {
        outLine = std::move(inputQueue_.front());
        inputQueue_.pop();
        return true;
    }

    return false;
}

char QuizGame::readAnswerWithTimeout(const Question& q) {
    std::string line;
    bool ok = popLineWithTimeout(q.timeoutSeconds, line);

    if (!ok) {
        std::cout << "\n" << RED << "Time out!!!" << COLOR_END << "\n";
        return 0;
    }

    /*
    line = trim(line);
    if (line.empty()) {
        return 0;
    }
    char ch = static_cast<char>(std::toupper(static_cast<unsigned char>(line[0])));
    */

    auto pos = line.find_first_not_of(" \t\r\n");
    if (pos == std::string::npos) {
        return 0;
    }

    char ch = static_cast<char>(
        std::toupper(static_cast<unsigned char>(line[pos]))
    );

    if (ch == 'A' || ch == 'B' || ch == 'C' || ch == 'D' || ch == 'L') {
        return ch;
    }

    return 0;
}

void QuizGame::printIntro() const {
    std::cout << "\n\n" << PINK << "\t\tLet's play Who Wants To Be A Quiz Champion" << COLOR_END << "\n";
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

    if (!popLineWithTimeout(24*60*60, line)) {
        return LifelineResult::None;
    }

    /*
    line = trim(line);
    if (line.empty()) {
        return LifelineResult::None;
    }
    */

    auto pos = line.find_first_not_of(" \t\r\n");
    if (pos == std::string::npos) {
        return LifelineResult::None;
    }

    char ch = line[pos];

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
    startInputThread();

    printIntro();

    for (Question& q : questions_) {
        bool skipThisQuestion = false;

        while (true) {
            printQuestion(q);
            char answer = readAnswerWithTimeout(q);

            if (answer == 0) {
                std::cout << "\n" << RED << "You failed to answer in time." << COLOR_END << "\n";
                printOutro();
                stopInputThread();
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
                stopInputThread();
                return;
            }
            break;
        }

        if (skipThisQuestion) {
            continue;
        }
    }

    std::cout << "\n" << BLUE 
            << "Congratulations! You finished all questions.\n" 
            << "Your total winnings are: $ " << moneyWon_ << COLOR_END << "\n";

    stopInputThread();
}
