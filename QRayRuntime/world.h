#pragma once
#include <vector>

struct Tile
{
    int x;
    int y;
};

extern const std::vector<Tile> worldWalls;

bool TileMatches(const Tile& tile, int x, int y);

int FindWall(int x, int y);