#pragma once
#include <string>
#include "Difficulty.h"

// Represents a single math question with its answer and difficulty.
class Question {
public:
    Question(const std::string& problemText, double answer, Difficulty difficulty);

    std::string getProblemText() const;
    double getAnswer() const;
    Difficulty getDifficulty() const;
    int getBasePoints() const;

    // Returns true if userAnswer is within an acceptable margin of the real answer.
    bool checkAnswer(double userAnswer) const;

private:
    std::string problemText_;
    double answer_;
    Difficulty difficulty_;
    int basePoints_;         // higher base points for harder difficulties
    double tolerance_;       // acceptable margin for floating-point answers
};
