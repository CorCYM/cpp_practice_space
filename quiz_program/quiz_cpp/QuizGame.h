#pragma once

#include "Question.h"
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <string>

class QuizGame {
public:
    explicit QuizGame(std::vector<Question> questions);
    ~QuizGame();
    void run();

private:
    enum class LifelineResult {
        None,
        FiftyFiftyUsed,
        SkipQuestion
    };

    std::vector<Question> questions_;
    int moneyWon_{0};
    int lifelineAvailable_[2]{1,1};

    void printIntro() const;
    void printOutro() const;
    void printQuestion(const Question& q) const;

    void startInputThread();
    void stopInputThread();
    void inputLoop();

    bool popLineWithTimeout(int timeoutSeconds, std::string& outLine);

    void clearInputQueue();

    char readAnswerWithTimeout(const Question& q);

    bool askRetry();

    LifelineResult useLifeline(Question& q);

    std::thread inputThread_;
    std::mutex inputMtx_;
    std::condition_variable inputCv_;
    std::queue<std::string> inputQueue_;
    std::atomic<bool> stop_{false};
};