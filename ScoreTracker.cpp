#include "ScoreTracker.h"
#include <iostream>

ScoreTracker::ScoreTracker()
    : player1Wins_(0), player2Wins_(0), totalRounds_(0) {
}

void ScoreTracker::recordWin(int playerNumber) {
    ++totalRounds_;
    if (playerNumber == 1) ++player1Wins_;
    else                   ++player2Wins_;
}

int ScoreTracker::getPlayer1Wins() const { return player1Wins_; }
int ScoreTracker::getPlayer2Wins() const { return player2Wins_; }
int ScoreTracker::getTotalRounds() const { return totalRounds_; }

void ScoreTracker::display() const {
    std::cout << "--- Score ---\n";
    std::cout << "Player 1 wins: " << player1Wins_ << "\n";
    std::cout << "Player 2 wins: " << player2Wins_ << "\n";
    std::cout << "Total rounds:  " << totalRounds_ << "\n";
}

void ScoreTracker::reset() {
    player1Wins_ = 0;
    player2Wins_ = 0;
    totalRounds_ = 0;
}
