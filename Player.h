#pragma once

#include "raylib.h"

class Player{
    private:
        int HP = 100;
        int status = 0;

        // =========================
        // ジャンプ関連
        // =========================

        // プレイヤーのY座標
        float playerY = 550.0f;
        float playerX = 300;

        // Y方向の速度
        float velocityY = 0.0f;

        // 重力
        float gravity = 700.0f;

        // ジャンプ力
        float jumpPower = -650.0f;

        // 地面のY座標
        float groundY = 550.0f;

        int isInvincible = 0;

        Sound jumpSound=LoadSound("assets/se_jump1.mp3");
        Sound hitSound=LoadSound("assets/se_power_up8.mp3");

    public:
        void startInputThread();

        int getHP();
        float getX();
        float getY();
        bool getStatus();
        bool isHitPuddle(float x, float y);
        bool isHitTrash(float x, float y);

        void update(float dt);
};