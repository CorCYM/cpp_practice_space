#pragma once

#include "Question.h"
#include <vector>

class QuizGame {
public:
    explicit QuizGame(std::vector<Question> questions);

    void run();

private:
    enum class LifelineResult {
        None,
        FiftyFiftyUsed,
        SkipQuestion
    };

    std::vector<Question> questions_;
    int moneyWon_{0};
    int lifelineAvailable_[2];

    void printIntro() const;
    void printOutro() const;
    void printQuestion(const Question& q) const;

    char readAnswerWithTimeout(const Question& q) const;
    LifelineResult useLifeline(Question& q);

    struct InputState;
};