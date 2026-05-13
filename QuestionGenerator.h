#pragma once
#include "Question.h"
#include "Difficulty.h"

// Factory class: creates Question objects based on the chosen difficulty.
// All question generation logic lives here, keeping Question as a plain data class.
class QuestionGenerator {
public:
    QuestionGenerator();

    Question generate(Difficulty difficulty) const;

private:
    Question generatePrimaryEasy() const;
    Question generatePrimaryHard() const;
    Question generateSecondaryEasy() const;
    Question generateSecondaryHard() const;
};
