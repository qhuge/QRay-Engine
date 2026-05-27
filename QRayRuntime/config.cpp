#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>

#include "config.h"
#include <fstream>

float playerX;

float playerY;

const int renderDistance = 35;

static const int WINDOW_WIDTH = 640;

static const int WINDOW_HEIGHT = 400;

const float scale = 8;

float angleOffset = 90.0f;

const float moveSpeed = 0.01f;

const float runSpeed = 0.02f;

const float angleSpeed = 0.5f;

const float lightDecay = 0.01f;

const float FOV = 80.0f;

GameConfig cfg;

bool InitConfig() {
    GameConfig config = {};

    std::ifstream file("data.qraydata", std::ios::binary);

    if (!file.is_open())
    {
        MessageBoxA(
            nullptr,
            "Failed to start",
            "Error",
            MB_OK);

        return false;
    }

    file.read((char*)&config, sizeof(GameConfig));

    file.close();

    cfg = config;

    return true;
}

std::vector<Texture> gTextures;
