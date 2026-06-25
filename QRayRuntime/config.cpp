#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>

#include "config.hpp"
#include <fstream>

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

std::vector<EntityType> gEntityTypes;

std::vector<int> gPlayerTags;
