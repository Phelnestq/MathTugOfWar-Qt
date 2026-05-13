#include "Question.h"
#include <cmath>  // for std::abs

// Base points awarded per difficulty — harder questions are worth more.
static int basePointsFor(Difficulty d) {
    switch (d) {
        case Difficulty::PRIMARY_EASY:   return 10;
        case Difficulty::PRIMARY_HARD:   return 20;
        case Difficulty::SECONDARY_EASY: return 30;
        case Difficulty::SECONDARY_HARD: return 50;
    }
    return 10;
}

Question::Question(const std::string& problemText, double answer, Difficulty difficulty)
    : problemText_(problemText),
      answer_(answer),
      difficulty_(difficulty),
      basePoints_(basePointsFor(difficulty)),
      tolerance_(0.01) {
}

std::string Question::getProblemText() const {
    return problemText_;
}

double Question::getAnswer() const {
    return answer_;
}

Difficulty Question::getDifficulty() const {
    return difficulty_;
}

int Question::getBasePoints() const {
    return basePoints_;
}

bool Question::checkAnswer(double userAnswer) const {
    return std::abs(userAnswer - answer_) <= tolerance_;
}
