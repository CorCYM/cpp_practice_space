#include "Question.h"
#include "QuizGame.h"

#include <iostream>
#include <cstdlib>
#include <ctime>

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    auto questions = loadQuestions("../questions_new.txt");
    if (questions.empty()) {
        std::cerr << "No questions loaded. Please check questions.txt format.\n";
        return 1;
    }

    QuizGame game(std::move(questions));
    game.run();

    return 0;
}