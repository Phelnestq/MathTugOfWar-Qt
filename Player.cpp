#include "Player.h"

Player::Player(const std::string& name)
    : name_(name), score_(0) {
}

std::string Player::getName() const {
    return name_;
}

int Player::getScore() const {
    return score_;
}

void Player::addScore(int points) {
    score_ += points;
}

void Player::resetScore() {
    score_ = 0;
}

bool Player::isComputer() const {
    return false;
}
