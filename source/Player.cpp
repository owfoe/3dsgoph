#include "Player.h"
#include "Ground.h"
#include "../include/Core.h"

Player::Player(float x, float y)
    : Entity(x, y, PlayerSettings::HEIGHT, PlayerSettings::WIDTH, PlayerSettings::HP, PlayerSettings::SPEED, PlayerSettings::COOLDOWN, AttackType::Bubble) {
    animator.add("idle", C2D_SpriteSheetLoad(Path::PLAYER_IDLE_SHEET), 6, LOOP);
    animator.add("run", C2D_SpriteSheetLoad(Path::PLAYER_MOVE_SHEET), 6, LOOP);
    animator.add("attk", C2D_SpriteSheetLoad(Path::PLAYER_ATTK_SHEET), 3, ONCE);
    animator.add("swd", C2D_SpriteSheetLoad(Path::PLAYER_SWD_SHEET), 6, ONCE);
    animator.play("idle");
    jumpSheet = C2D_SpriteSheetLoad(Path::PLAYER_JUMP_SHEET);
    setScaleX(1.0f);
}

void Player::jump()
{
    if (groundPlatform != nullptr)
    {
        if (std::abs(vx) != speed)
            vy = 7.5f;
        else
            vy = 9.5f;
    }
    isJump = true;
    // onGround = false;
    fallLogic.setIsFall(false);
    groundPlatform = nullptr;
}

void Player::updateJump(bool jumpButtonDown)
{
    if (isJump && groundPlatform == nullptr)
    {
        if (jumpButtonDown && vy > 0.0f)
            vy -= jumpGravityLow;
        else
            vy -= jumpGravityHigh;
    }
}

void Player::update(bool jumpButtonDown)
{
    checkDeath();
    updateJump(jumpButtonDown);
    state = getState();
    bool isOnGround = (groundPlatform == nullptr) ? false : true;
    if (isOnGround) isJump = false;
    vy = fallLogic.updateFall(isOnGround, isJump, vy);
    if (isSwd) {
        animator.play("swd");
        if (animator.isFinished()) {
            isSwd = false;
            isAttacking = false;
            animator.play(state == EntityActionState::Run ? "run" : "idle");
        }
    }
    else if (isAttacking) {
        animator.play("attk");
        if (animator.isFinished())
            isAttacking = false;
    }
    else if (state == EntityActionState::Run)
    {
        animator.play("run");
    }
    else
    {
        animator.play("idle");
    }

    animator.update();

}

void Player::draw(float cameraPos, int layer) {
    // C2D_DrawRectSolid(x - cameraPos, y, layer, width, height, C2D_Color32f(1, 0, 0, 1));
    if (isJump) {
        if (vy > 0.0f) currentImage = C2D_SpriteSheetGetImage(jumpSheet, 0);
        else if (vy < 0.0f) currentImage = C2D_SpriteSheetGetImage(jumpSheet, 1);
    }
    else {
        currentImage = animator.getImage();
    }


    if (getScaleX() > 0) {
        C2D_DrawImageAt(currentImage, x - (cameraPos + 55), y - 12, layer, nullptr, 1.0f, 1.0f);
    }
    else {
        if (isSwd) C2D_DrawImageAt(currentImage, x - (cameraPos + 49), y - 12, layer, nullptr, -1.0f, 1.0f);
        else C2D_DrawImageAt(currentImage, x - (cameraPos + 17), y - 12, layer, nullptr, -1.0f, 1.0f);
    }


}

void Player::erasePowerups()
{
    powerups.erase(
        std::remove_if(powerups.begin(), powerups.end(),
                       [](const Powerup &pu)
                       {
                           return pu.getIsDead();
                       }),
        powerups.end());
}

void Player::usePowerup(int index)
{
    Powerup &pu = powerups[index];

    attackType = pu.getType();
    isUsingPowerup = true;
    endUsingPowerup = pu.getEndUsing();

    powerups.erase(powerups.begin() + index);
}

void Player::updatePowerup(uint64_t timer)
{
    if (isUsingPowerup && timer >= endUsingPowerup)
    {
        isUsingPowerup = false;
        attackType = AttackType::Bubble;
    }
}

void Player::pickUpPowerup(std::vector<Powerup>::iterator pu)
{
    powerups.insert(powerups.begin(), std::move(*pu));
    int s = powerups.size();
    while (s > 2)
    {
        powerups.pop_back();
        s -= 1;
    }
    for (int i = 0; i < s; i++)
    {
        powerups.at(i).setX(95 + i * 100);
        powerups.at(i).setY(93);
    }
}

uint64_t Player::getAttackStartup(AttackType type)
{
    switch (type)
    {
    case AttackType::Sword:
        return 72;
    case AttackType::Shot:
        return 0;
    case AttackType::Bubble:
        return 0;
    default:
        return 0;
    }
}

void Player::freeJumpSheet() {
    C2D_SpriteSheetFree(jumpSheet);
}

void Player::onAttackStart(AttackType type)
{
    if (type == AttackType::Sword)
    {
        setSwd(true);   // or animator.play("attk")
    }
}
