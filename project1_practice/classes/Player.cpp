#include "Player.hpp"

Player::Player(float x, float y, float angle, float speed)
    : x(x), y(y), angle(angle), speed(speed) {}

float Player::getX() const {
    return x;
}

float Player::getY() const {
    return y;
}

float Player::getAngle() const {
    return angle;
}

float Player::getSpeed() const {
    return speed;
}

void Player::setX(float x) {
    this->x = x;
}

void Player::setY(float y) {
    this->y = y;
}

void Player::setAngle(float angle) {
    this->angle = angle;
}

void Player::setSpeed(float speed) {
    this->speed = speed;
}