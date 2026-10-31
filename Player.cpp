#include "Player.h"
#include "raylib.h"
#include <iostream>

void Player::update(float dt)
{
    char left_hand, right_hand;
    std::cin >> left_hand >> right_hand;

    bool isUpDown = IsKeyDown(KEY_UP) || (left_hand == 'o');
    bool isSpaceDown = IsKeyPressed(KEY_SPACE) || (right_hand == 'o');

    if(status != 0)
    {
        status--;
    }
    else if (isUpDown)
    {
        status += 30;
    }

    // -------------------------
    // ジャンプ
    // -------------------------

    if (isSpaceDown && playerY >= groundY)
    {
        velocityY = jumpPower;

        // ジャンプ音
        PlaySound(jumpSound);
    }

    // 重力
    velocityY += gravity * dt;

    // Y座標を更新
    playerY += velocityY * dt;

    if(isInvincible > 0)
    {
        isInvincible--;
    }

    // 地面より下に行かないようにする
    if (playerY >= groundY)
    {
        playerY = groundY;
        velocityY = 0.0f;
    }
}

int Player::getHP()
{
    return HP;
}

bool Player::getStatus()
{
    return (status != 0);
}

float Player::getX()
{
    return playerX;
}

float Player::getY()
{
    return playerY;
}

bool Player::isHitPuddle(float x, float y)
{
    float hitWidth = 50.0f;
    float hitHeight = 60.0f;

    if (
        playerX - hitWidth < x &&
        x < playerX + hitWidth &&
        playerY - hitHeight < y &&
        y < playerY + hitHeight &&
        isInvincible == 0
    )
    {
        HP -= 20;
        isInvincible += 40;
        PlaySound(hitSound);
        return true;
    }

    return false;
}

bool Player::isHitTrash(float x, float y)
{
    float hitWidth = 50.0f;
    float hitHeight = 60.0f;

    if (
        playerX - hitWidth < x &&
        x < playerX + hitWidth &&
        playerY - hitHeight < y &&
        y < playerY + hitHeight &&
        isInvincible == 0
    )
    {
        if(status != 0)
        {
            return true;
        }

        HP -= 10;
        isInvincible += 40;
        PlaySound(hitSound);
        return true;
    }

    return false;
}