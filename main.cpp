#include <raylib.h>
#include <raymath.h>
#include <array>
#include <stdio.h>
#include <cstdint>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <pnm.h>

struct Entity {
    Vector2 position;
    Vector2 direction;
    float speed;
    Texture2D texture;
    Rectangle* sprites;
    int spritesCount;
    int spriteIdx;
    int spriteFrames;
};

void AnimateEntity(Entity* e, int frame) {
    e->spriteIdx = (frame / e->spriteFrames) % e->spritesCount;
}

void DrawEntity(Entity* e) {
    DrawTextureRec(e->texture, e->sprites[e->spriteIdx], e->position, BLACK);
}

void MoveEntity(Entity* e, float deltaT) {
    e->position = Vector2Add(e->position, Vector2Scale(e->direction, e->speed*deltaT));
}

int main() {
    static constexpr int screenWidth = 800;
    static constexpr int screenHeight = 450;
    static constexpr int tileSize = 32;
    static constexpr int FPS = 60;

    InitWindow(screenWidth, screenHeight, "pacman");
    SetTargetFPS(FPS);

    Rectangle pacmanSprites[] = {
        {.x = 0, .y = 0, .width = tileSize, .height = tileSize},
        {.x = 0, .y = 32, .width = tileSize, .height = tileSize},
        {.x = 0, .y = 64, .width = tileSize, .height = tileSize},
        {.x = 0, .y = 96, .width = tileSize, .height = tileSize},
        {.x = 0, .y = 64, .width = tileSize, .height = tileSize},
        {.x = 0, .y = 32, .width = tileSize, .height = tileSize},
    };

    Entity pacman = {
        .position = {.x = 0, .y = 0},
        .direction = {.x = 0, .y = 0},
        .speed = 96.,
        .texture = LoadTexture(ASSETS_PATH "/pacman.png"),
        .sprites = pacmanSprites,
        .spritesCount = 6,
        .spriteIdx = 0,
        .spriteFrames = 10,
    };

    PNM level0 = LoadPNM(ASSETS_PATH "/level0.pgm");
    for (int r = 0; r < level0.height; r++) {
        for (int c = 0; c < level0.width; c++) {
            if (PNMGetGray(&level0, r, c)) {
                printf("x");
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }

    int frame = 0;

    while(!WindowShouldClose()) {
        pacman.direction.x = 0;
        pacman.direction.y = 0;

        if (IsKeyDown(KEY_LEFT)) {
            pacman.direction.x += -1;
        }
        if (IsKeyDown(KEY_RIGHT)) {
            pacman.direction.x += 1;
        }
        if (IsKeyDown(KEY_DOWN)) {
            pacman.direction.y += 1;
        }
        if (IsKeyDown(KEY_UP)) {
            pacman.direction.y += -1;
        }

        pacman.direction = Vector2Normalize(pacman.direction);

        float deltaT = GetFrameTime();
        MoveEntity(&pacman, deltaT);
        AnimateEntity(&pacman, frame);

        BeginDrawing(); {
            ClearBackground(BLACK); 
            DrawEntity(&pacman);
        } EndDrawing();

        frame++;
    }

    UnloadPNM(&level0);
    CloseWindow();
}