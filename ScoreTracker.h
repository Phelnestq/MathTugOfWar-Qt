#pragma once

// Tracks round wins across multiple games.
// Separate from Player score so per-round and overall stats are distinct.
class ScoreTracker {
public:
    ScoreTracker();

    // Record a round win. playerNumber: 1 or 2.
    void recordWin(int playerNumber);

    int getPlayer1Wins() const;
    int getPlayer2Wins() const;
    int getTotalRounds() const;

    void display() const;
    void reset();

private:
    int player1Wins_;
    int player2Wins_;
    int totalRounds_;
};
