#include "raylib.h"
#include "Player.h"
#include <vector>
#include <algorithm>

struct Puddle
{
    float x;
    float scale;
};

struct Trash
{
    float x;
    float y;
    float scale;

    bool hit = false;
};

int main(void)
{
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "rain");
    InitAudioDevice();

    // =========================
    // 画像読み込み
    // =========================

    Texture2D background = LoadTexture("assets/city.png");

    Texture2D players[] = {
        LoadTexture("assets/fire1.png"),
        LoadTexture("assets/fire2.png"),
        LoadTexture("assets/block.png")
    };

    Texture2D puddleTexture = LoadTexture("assets/puddle.png");
    Texture2D trashTexture = LoadTexture("assets/trash.png");

    Music bgm1 = LoadMusicStream("assets/rain.mp3");
    Music bgm2 = LoadMusicStream("assets/music.mp3");

    SetMusicVolume(bgm1, 0.5f);
    SetMusicVolume(bgm2, 0.3f);

    PlayMusicStream(bgm1);
    PlayMusicStream(bgm2);

    float puddleW = puddleTexture.width;

    // =========================
    // 背景
    // =========================

    float scrollingBack = 0.0f;
    float scrollSpeed = 150.0f;

    float bgScale = 0.9f;
    float bgWidth = background.width * bgScale;

    SetTargetFPS(60);

    // =========================
    // 各種変数
    // =========================

    int frameCount = 0;

    float puddleTimer = 0.0f;
    float trashTimer = 0.0f;

    Player chara;
    chara.startInputThread();

    std::vector<Puddle> puddles;
    std::vector<Trash> trashes;

    // =========================
    // 雨
    // =========================

    const int rainCount = 300;
    Vector2 rain[rainCount];

    for (int i = 0; i < rainCount; i++)
    {
        rain[i] = {
            (float)GetRandomValue(0, screenWidth),
            (float)GetRandomValue(-screenHeight, screenHeight)
        };
    }

    // =========================
    // メインループ
    // =========================

    while (!WindowShouldClose())
    {
        UpdateMusicStream(bgm1);
        UpdateMusicStream(bgm2);

        float dt = GetFrameTime() * 2;

        // =========================
        // プレイヤー
        // =========================

        chara.update(dt);

        // =========================
        // 水たまり更新
        // =========================

        for (auto& puddle : puddles)
        {
            // 水たまりの左右を当たり判定
            if (
                chara.isHitPuddle(puddle.x, 600.0f) ||
                chara.isHitPuddle(
                    puddle.x + puddleW * puddle.scale - 40.0f,
                    600.0f
                )
            )
            {
                TraceLog(
                    LOG_INFO,
                    "HIT! HP = %d",
                    chara.getHP()
                );
            }

            puddle.x -= scrollSpeed * dt;
        }

        // 画面外の水たまりを削除
        puddles.erase(
            std::remove_if(
                puddles.begin(),
                puddles.end(),
                [](const Puddle& puddle)
                {
                    return puddle.x < -300.0f;
                }
            ),
            puddles.end()
        );

        // =========================
        // ゴミ更新
        // =========================

        for (auto& trash : trashes)
        {
            if(chara.isHitTrash(trash.x, trash.y)){
                trash.hit = true;
            }
            
            // ゴミは水たまりより速く飛んでくる
            trash.x -= 300.0f * dt;
        }

        // 画面外のゴミを削除
        trashes.erase(
            std::remove_if(
                trashes.begin(),
                trashes.end(),
                [](const Trash& trash)
                {
                    return trash.hit || trash.x < -300.0f;
                }
            ),
            trashes.end()
        );

        // =========================
        // 背景スクロール
        // =========================

        scrollingBack -= scrollSpeed * dt;

        if (scrollingBack <= -bgWidth)
        {
            scrollingBack = 0.0f;
        }

        // =========================
        // 雨
        // =========================

        for (int i = 0; i < rainCount; i++)
        {
            rain[i].y += 500.0f * dt;

            if (rain[i].y > screenHeight)
            {
                rain[i].x = GetRandomValue(0, screenWidth);
                rain[i].y = GetRandomValue(-100, 0);
            }
        }

        // =========================
        // 水たまり生成
        // =========================

        puddleTimer -= dt;

        if (puddleTimer <= 0)
        {
            puddles.push_back({
                screenWidth + 100.0f,
                GetRandomValue(20, 30) / 10.0f
            });

            // 次の水たまりまでの時間
            puddleTimer = GetRandomValue(10, 30) / 5.0f;
        }

        // =========================
        // ゴミ生成
        // =========================

        trashTimer -= dt;

        if (trashTimer <= 0)
        {
            trashes.push_back({
                (float)screenWidth + 100.0f,
                (float)GetRandomValue(300, 500),
                GetRandomValue(10, 20) / 10.0f
            });

            // 次のゴミまでの時間
            trashTimer = GetRandomValue(10, 25) / 10.0f;
        }

        // =========================
        // 描画
        // =========================

        BeginDrawing();

            ClearBackground(RAYWHITE);

            // =========================
            // 背景
            // =========================

            DrawTextureEx(
                background,
                (Vector2){ scrollingBack, -250 },
                0.0f,
                bgScale,
                WHITE
            );

            DrawTextureEx(
                background,
                (Vector2){ scrollingBack + bgWidth, -250 },
                0.0f,
                bgScale,
                WHITE
            );

            // =========================
            // 雨
            // =========================

            for (int i = 0; i < rainCount; i++)
            {
                DrawLineEx(
                    rain[i],
                    (Vector2){
                        rain[i].x - 5,
                        rain[i].y + 25
                    },
                    2.0f,
                    Fade(SKYBLUE, 0.7f)
                );
            }

            // =========================
            // 水たまり
            // =========================

            for (const auto& puddle : puddles)
            {
                DrawTextureEx(
                    puddleTexture,
                    { puddle.x, 600.0f },
                    0.0f,
                    puddle.scale,
                    WHITE
                );
            }

            // =========================
            // ゴミ
            // =========================

            for (const auto& trash : trashes)
            {
                DrawTextureEx(
                    trashTexture,
                    { trash.x, trash.y },
                    0.0f,
                    trash.scale,
                    WHITE
                );
            }

            // =========================
            // HPバー
            // =========================

            const int hp = chara.getHP();
            const int maxHP = 100;

            float hpRate = (float)hp / maxHP;

            DrawRectangle(
                30,
                30,
                250,
                25,
                DARKGRAY
            );

            DrawRectangle(
                30,
                30,
                250 * hpRate,
                25,
                RED
            );

            DrawRectangleLines(
                30,
                30,
                250,
                25,
                WHITE
            );

            // =========================
            // プレイヤー
            // =========================

            Texture2D player;

            float scale = 3.0f;

            if (chara.getStatus())
            {
                player = players[2];
                scale = 4.0f;
            }
            else
            {
                player = players[frameCount / 20 % 2];
            }

            float width = player.width * scale;
            float height = player.height * scale;

            DrawTextureEx(
                player,
                {
                    chara.getX() - width / 2,
                    chara.getY() - height / 2
                },
                0.0f,
                scale,
                WHITE
            );

                    if (chara.getHP() <= 0)
        {
            DrawText(
                "GAME OVER",
                screenWidth / 2 - 180,
                screenHeight / 2 - 50,
                70,
                RED
            );
        }

        EndDrawing();

        frameCount++;
    }

    // =========================
    // 終了処理
    // =========================

    UnloadTexture(background);

    UnloadTexture(players[0]);
    UnloadTexture(players[1]);
    UnloadTexture(players[2]);

    UnloadTexture(puddleTexture);
    UnloadTexture(trashTexture);

    CloseWindow();

    return 0;
}