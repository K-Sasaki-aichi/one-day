#include "raylib.h"

#include <cmath>
#include <vector>
#include <algorithm>

struct Trash
{
    Vector2 position;
    float speed;
    float rotation;
    bool alive;
};

struct Puddle
{
    float x;
    float width;
    bool alive;
};

struct RainDrop
{
    float x;
    float y;
    float speed;
};

int main()
{
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Rain Rhythm");

    SetTargetFPS(60);

    // =========================================================
    // プレイヤー
    // =========================================================

    const float playerX = screenWidth / 2.0f;
    const float groundY = 580.0f;

    float playerY = groundY;
    float velocityY = 0.0f;

    const float gravity = 1600.0f;
    const float jumpPower = -700.0f;

    bool onGround = true;

    int hp = 100;
    int score = 0;

    // =========================================================
    // 傘
    // =========================================================

    // false = 正面
    // true  = 横向き
    bool umbrellaSideways = false;

    // ゴミを防御した直後の演出
    float blockEffectTimer = 0.0f;

    // 横向きにしたが、防御対象がなかった場合の判定
    float sidewaysTimer = 0.0f;

    // =========================================================
    // ゴミ
    // =========================================================

    std::vector<Trash> trash;

    float trashSpawnTimer = 0.0f;

    // 最初は少し簡単にする
    const float trashSpawnInterval = 1.8f;

    // =========================================================
    // 水たまり
    // =========================================================

    std::vector<Puddle> puddles;

    float puddleSpawnTimer = 0.0f;
    const float puddleSpawnInterval = 3.5f;

    // =========================================================
    // 雨
    // =========================================================

    std::vector<RainDrop> rain;

    for (int i = 0; i < 300; i++)
    {
        RainDrop r;

        r.x = (float)GetRandomValue(0, screenWidth);
        r.y = (float)GetRandomValue(0, screenHeight);

        r.speed = (float)GetRandomValue(500, 900);

        rain.push_back(r);
    }

    // =========================================================
    // 背景スクロール
    // =========================================================

    float backgroundOffset = 0.0f;

    const float backgroundSpeed = 250.0f;

    // =========================================================
    // ダメージ演出
    // =========================================================

    float damageEffectTimer = 0.0f;

    // =========================================================
    // ゲームループ
    // =========================================================

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // =====================================================
        // GAME OVER中
        // =====================================================

        if (hp > 0)
        {
            // =================================================
            // 背景をスクロール
            // =================================================

            backgroundOffset += backgroundSpeed * dt;

            if (backgroundOffset >= 100.0f)
            {
                backgroundOffset -= 100.0f;
            }

            // =================================================
            // 雨
            // =================================================

            for (auto &r : rain)
            {
                r.y += r.speed * dt;

                if (r.y > screenHeight)
                {
                    r.y = -20.0f;
                    r.x = (float)GetRandomValue(0, screenWidth);
                }
            }

            // =================================================
            // ジャンプ
            // =================================================

            if (IsKeyPressed(KEY_SPACE) && onGround)
            {
                velocityY = jumpPower;
                onGround = false;
            }

            // 重力
            velocityY += gravity * dt;
            playerY += velocityY * dt;

            // 地面
            if (playerY >= groundY)
            {
                playerY = groundY;
                velocityY = 0.0f;
                onGround = true;
            }

            // =================================================
            // 傘操作
            // =================================================

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                // 正面 → 横
                if (!umbrellaSideways)
                {
                    umbrellaSideways = true;
                    sidewaysTimer = 0.0f;
                }
                // 横 → 正面
                else
                {
                    umbrellaSideways = false;
                    sidewaysTimer = 0.0f;
                }
            }

            // 横向きになっている時間
            if (umbrellaSideways)
            {
                sidewaysTimer += dt;
            }

            // =================================================
            // ゴミ生成
            // =================================================

            trashSpawnTimer += dt;

            if (trashSpawnTimer >= trashSpawnInterval)
            {
                trashSpawnTimer = 0.0f;

                Trash t;

                // 左右どちらかから飛んでくる
                bool fromLeft = GetRandomValue(0, 1) == 0;

                if (fromLeft)
                {
                    t.position.x = -50.0f;
                }
                else
                {
                    t.position.x = screenWidth + 50.0f;
                }

                // ゴミは主人公の少し上を飛ぶ
                t.position.y =
                    playerY - (float)GetRandomValue(70, 150);

                t.speed =
                    (float)GetRandomValue(350, 500);

                if (!fromLeft)
                {
                    t.speed *= -1.0f;
                }

                t.rotation =
                    (float)GetRandomValue(0, 360);

                t.alive = true;

                trash.push_back(t);
            }

            // =================================================
            // 水たまり生成
            // =================================================

            puddleSpawnTimer += dt;

            if (puddleSpawnTimer >= puddleSpawnInterval)
            {
                puddleSpawnTimer = 0.0f;

                Puddle p;

                p.x =
                    (float)screenWidth + 100.0f;

                p.width =
                    (float)GetRandomValue(70, 120);

                p.alive = true;

                puddles.push_back(p);
            }

            // =================================================
            // ゴミの移動
            // =================================================

            for (auto &t : trash)
            {
                if (!t.alive)
                    continue;

                t.position.x += t.speed * dt;

                t.rotation += 180.0f * dt;

                // -------------------------------------------------
                // ゴミが主人公の近くまで来た
                // -------------------------------------------------

                float distanceX =
                    fabsf(t.position.x - playerX);

                float distanceY =
                    fabsf(t.position.y - (playerY - 60.0f));

                // ゴミが傘の防御範囲に入った
                if (distanceX < 80.0f &&
                    distanceY < 80.0f)
                {
                    if (umbrellaSideways)
                    {
                        // =========================================
                        // 防御成功
                        // =========================================

                        t.alive = false;

                        score += 100;

                        blockEffectTimer = 0.25f;

                        // 防御成功したら自動的に正面へ
                        umbrellaSideways = false;

                        sidewaysTimer = 0.0f;
                    }
                    else
                    {
                        // =========================================
                        // 傘を横にしていなかった
                        // =========================================

                        t.alive = false;

                        hp -= 10;

                        damageEffectTimer = 0.25f;
                    }
                }
            }

            // =================================================
            // ゴミ削除
            // =================================================

            trash.erase(
                std::remove_if(
                    trash.begin(),
                    trash.end(),
                    [](const Trash &t)
                    {
                        return !t.alive ||
                               t.position.x < -150.0f ||
                               t.position.x > 1430.0f;
                    }
                ),
                trash.end()
            );

            // =================================================
            // 水たまりをスクロール
            // =================================================

            for (auto &p : puddles)
            {
                if (!p.alive)
                    continue;

                p.x -= backgroundSpeed * dt;

                // ---------------------------------------------
                // 水たまりに接近
                // ---------------------------------------------

                float playerLeft =
                    playerX - 20.0f;

                float playerRight =
                    playerX + 20.0f;

                float puddleLeft =
                    p.x;

                float puddleRight =
                    p.x + p.width;

                bool horizontalHit =
                    playerRight > puddleLeft &&
                    playerLeft < puddleRight;

                if (horizontalHit && onGround)
                {
                    // 水たまりに入った
                    p.alive = false;

                    hp -= 5;

                    damageEffectTimer = 0.2f;
                }

                // ジャンプしていた場合は何も起こらない
                if (p.x + p.width < -100.0f)
                {
                    p.alive = false;
                }
            }

            // =================================================
            // 水たまり削除
            // =================================================

            puddles.erase(
                std::remove_if(
                    puddles.begin(),
                    puddles.end(),
                    [](const Puddle &p)
                    {
                        return !p.alive;
                    }
                ),
                puddles.end()
            );

            // =================================================
            // 演出タイマー
            // =================================================

            if (blockEffectTimer > 0.0f)
            {
                blockEffectTimer -= dt;
            }

            if (damageEffectTimer > 0.0f)
            {
                damageEffectTimer -= dt;
            }
        }

        // =====================================================
        // 描画
        // =====================================================

        BeginDrawing();

        // =====================================================
        // 背景
        // =====================================================

        ClearBackground(
            Color{ 35, 45, 60, 255 }
        );

        // =====================================================
        // 遠くの建物
        // =====================================================

        for (int i = -1; i < 15; i++)
        {
            float x =
                i * 100.0f -
                backgroundOffset;

            int height =
                120 + (i % 3) * 50;

            DrawRectangle(
                (int)x,
                (int)(groundY - height),
                80,
                height,
                Color{ 45, 55, 70, 255 }
            );

            // 窓
            for (int y = 0; y < height - 20; y += 35)
            {
                DrawRectangle(
                    (int)x + 15,
                    (int)(groundY - height + y + 10),
                    12,
                    15,
                    Color{ 100, 110, 120, 150 }
                );

                DrawRectangle(
                    (int)x + 50,
                    (int)(groundY - height + y + 10),
                    12,
                    15,
                    Color{ 100, 110, 120, 150 }
                );
            }
        }

        // =====================================================
        // 雨
        // =====================================================

        for (const auto &r : rain)
        {
            DrawLine(
                r.x,
                r.y,
                r.x - 5,
                r.y + 20,
                Color{ 130, 170, 200, 130 }
            );
        }

        // =====================================================
        // 道路
        // =====================================================

        DrawRectangle(
            0,
            (int)groundY,
            screenWidth,
            screenHeight - (int)groundY,
            Color{ 50, 55, 60, 255 }
        );

        // 道路の線
        for (int i = -1; i < 15; i++)
        {
            float x =
                i * 100.0f -
                backgroundOffset;

            DrawRectangle(
                (int)x,
                (int)groundY + 65,
                60,
                6,
                Color{ 100, 100, 100, 255 }
            );
        }

        // =====================================================
        // 水たまり
        // =====================================================

        for (const auto &p : puddles)
        {
            DrawEllipse(
                (int)(p.x + p.width / 2),
                (int)groundY + 5,
                p.width / 2,
                9,
                Color{ 60, 120, 170, 180 }
            );
        }

        // =====================================================
        // ゴミ
        // =====================================================

        for (const auto &t : trash)
        {
            // 回転するゴミ
            Vector2 center = t.position;

            DrawRectanglePro(
                {
                    center.x,
                    center.y,
                    30,
                    20
                },
                {
                    15,
                    10
                },
                t.rotation,
                Color{ 170, 140, 100, 255 }
            );

            DrawLine(
                (int)center.x - 8,
                (int)center.y - 4,
                (int)center.x + 8,
                (int)center.y + 4,
                BLACK
            );
        }

        // =====================================================
        // プレイヤー
        // =====================================================

        Color playerColor =
            Color{ 230, 230, 230, 255 };

        if (damageEffectTimer > 0.0f)
        {
            playerColor = RED;
        }

        // 足
        DrawRectangle(
            (int)playerX - 12,
            (int)playerY - 10,
            8,
            10,
            playerColor
        );

        DrawRectangle(
            (int)playerX + 4,
            (int)playerY - 10,
            8,
            10,
            playerColor
        );

        // 体
        DrawRectangle(
            (int)playerX - 15,
            (int)playerY - 55,
            30,
            45,
            playerColor
        );

        // 頭
        DrawCircle(
            (int)playerX,
            (int)playerY - 70,
            15,
            playerColor
        );

        // =====================================================
        // 傘
        // =====================================================

        float umbrellaX = playerX;

        float umbrellaY = playerY - 95;

        if (!umbrellaSideways)
        {
            // -------------------------------------------------
            // 正面
            // -------------------------------------------------

            // 傘
            DrawCircle(
                (int)umbrellaX,
                (int)umbrellaY,
                55,
                Color{ 70, 110, 180, 255 }
            );

            // 下半分を背景色で消す
            DrawRectangle(
                (int)umbrellaX - 60,
                (int)umbrellaY,
                120,
                60,
                Color{ 35, 45, 60, 255 }
            );

            // 傘の柄
            DrawLineEx(
                {
                    umbrellaX,
                    umbrellaY
                },
                {
                    umbrellaX,
                    playerY - 30
                },
                5,
                WHITE
            );
        }
        else
        {
            // -------------------------------------------------
            // 横向き
            // -------------------------------------------------

            // 傘の横向き部分
            DrawRectanglePro(
                {
                    umbrellaX,
                    umbrellaY,
                    100,
                    45
                },
                {
                    50,
                    22
                },
                0,
                Color{ 70, 110, 180, 255 }
            );

            // 傘の先端
            DrawCircle(
                (int)(umbrellaX + 50),
                (int)umbrellaY,
                8,
                Color{ 50, 80, 140, 255 }
            );

            // 柄
            DrawLineEx(
                {
                    umbrellaX,
                    umbrellaY
                },
                {
                    umbrellaX - 40,
                    playerY - 35
                },
                5,
                WHITE
            );
        }

        // =====================================================
        // 防御成功エフェクト
        // =====================================================

        if (blockEffectTimer > 0.0f)
        {
            DrawCircleLines(
                (int)playerX,
                (int)(playerY - 70),
                70,
                WHITE
            );

            DrawText(
                "BLOCK!",
                (int)playerX - 50,
                (int)playerY - 160,
                25,
                WHITE
            );
        }

        // =====================================================
        // UI
        // =====================================================

        DrawText(
            TextFormat("HP: %d", hp),
            30,
            25,
            30,
            WHITE
        );

        DrawText(
            TextFormat("SCORE: %d", score),
            30,
            65,
            25,
            WHITE
        );

        DrawText(
            "LEFT CLICK : Umbrella",
            30,
            screenHeight - 70,
            22,
            WHITE
        );

        DrawText(
            "SPACE : Jump",
            30,
            screenHeight - 40,
            22,
            WHITE
        );

        // =====================================================
        // GAME OVER
        // =====================================================

        if (hp <= 0)
        {
            DrawRectangle(
                0,
                0,
                screenWidth,
                screenHeight,
                Color{ 0, 0, 0, 170 }
            );

            DrawText(
                "GAME OVER",
                screenWidth / 2 - 150,
                screenHeight / 2 - 50,
                50,
                RED
            );

            DrawText(
                TextFormat("SCORE: %d", score),
                screenWidth / 2 - 70,
                screenHeight / 2 + 20,
                25,
                WHITE
            );
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}