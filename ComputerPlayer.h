#pragma once
#include "Player.h"
#include "Difficulty.h"

class Question;  // forward declaration avoids circular includes

// Inherits from Player — IS-A Player, but answers automatically.
// Difficulty controls how fast and how accurately the computer responds.
class ComputerPlayer : public Player {
public:
    ComputerPlayer(const std::string& name, Difficulty difficulty);

    bool isComputer() const override;

    // Returns a simulated answer time in seconds based on difficulty.
    double simulateAnswerTime() const;

    // Returns the computer's answer attempt for a question.
    // May be wrong on lower difficulties to keep the game fair.
    double simulateAnswer(const Question& question) const;

private:
    Difficulty difficulty_;
};
