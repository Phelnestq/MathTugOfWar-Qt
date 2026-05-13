#include "ComputerPlayer.h"
#include "Question.h"
#include <cstdlib>  // for rand()

ComputerPlayer::ComputerPlayer(const std::string& name, Difficulty difficulty)
    : Player(name), difficulty_(difficulty) {
}

bool ComputerPlayer::isComputer() const {
    return true;
}

// Simulates how long the computer takes to answer.
// Harder difficulties respond faster, giving them stronger pulls.
double ComputerPlayer::simulateAnswerTime() const {
    // Base time ranges (seconds): faster = more pull via base_points / time
    switch (difficulty_) {
        case Difficulty::PRIMARY_EASY:    return 4.0 + (rand() % 30) / 10.0;  // 4.0 – 7.0s
        case Difficulty::PRIMARY_HARD:    return 3.0 + (rand() % 20) / 10.0;  // 3.0 – 5.0s
        case Difficulty::SECONDARY_EASY:  return 2.5 + (rand() % 20) / 10.0;  // 2.5 – 4.5s
        case Difficulty::SECONDARY_HARD:  return 1.5 + (rand() % 20) / 10.0;  // 1.5 – 3.5s
    }
    return 5.0;
}

// Simulates the computer's answer. Harder difficulties answer correctly more often.
double ComputerPlayer::simulateAnswer(const Question& question) const {
    int accuracyRoll = rand() % 100;

    // Accuracy thresholds per difficulty (percentage chance of correct answer)
    int correctThreshold = 0;
    switch (difficulty_) {
        case Difficulty::PRIMARY_EASY:   correctThreshold = 60; break;
        case Difficulty::PRIMARY_HARD:   correctThreshold = 75; break;
        case Difficulty::SECONDARY_EASY: correctThreshold = 85; break;
        case Difficulty::SECONDARY_HARD: correctThreshold = 95; break;
    }

    if (accuracyRoll < correctThreshold) {
        return question.getAnswer();  // correct
    }

    // Wrong answer: offset the real answer by a small random amount
    double offset = (rand() % 5) + 1;
    return question.getAnswer() + offset;
}
