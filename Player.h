#pragma once
#include <string>

// Base class representing a human player.
// ComputerPlayer inherits from this class.
class Player {
public:
    Player(const std::string& name);
    virtual ~Player() = default;

    std::string getName() const;
    int getScore() const;
    void addScore(int points);
    void resetScore();

    // Overridden by ComputerPlayer to return true.
    virtual bool isComputer() const;

protected:
    std::string name_;
    int score_;
};
