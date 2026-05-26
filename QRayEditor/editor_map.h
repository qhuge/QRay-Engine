#pragma once

#include "config.h"
#include <vector>

struct Tile
{
    int x;
    int y;
    int textureIndex;
};
extern std::vector<Tile> worldMap;

void SaveMap();
void LoadMap();

int FindWall(int x, int y);