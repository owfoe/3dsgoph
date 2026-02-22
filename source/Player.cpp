#include "Player.h"

Player::Player(float x, float y, float height, float width, int hp, float speed)
    : Entity(x, y, height, width, hp, speed), raycast(height, width) {}

void Player::jump() {}
void Player::updateJump() {}