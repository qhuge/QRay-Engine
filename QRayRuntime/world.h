#pragma once
#include <vector>
#include "config.h"
#include <string>

struct Tile
{
    int x;
    int y;
    int textureIndex;
};

extern std::vector<Tile> worldWalls;

bool TileMatches(const Tile& tile, int x, int y);

int FindWall(int x, int y);

bool LoadWorld(const char* filename);

Texture LoadQRayAsset(std::string path);