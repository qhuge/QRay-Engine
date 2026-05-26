#include "world.h"
#include <fstream>

//const std::vector<Tile> worldWalls =
//{
//    {100,104, 0},
//    {101,105, 0},
//    {99,105, 0},
//    {99,103, 0},
//    {101,103, 0},
//};

std::vector<Tile> worldWalls;

bool TileMatches(const Tile& tile, int x, int y)
{
    return tile.x == x &&
        tile.y == y;
}

int FindWall(int x, int y)
{
    for (int i = 0; i < worldWalls.size(); i++)
    {
        if (worldWalls[i].x == x &&
            worldWalls[i].y == y)
        {
            return i;
        }
    }
    return -1;
}

bool LoadWorld(const char* filename)
{
    worldWalls.clear();

    std::ifstream file(filename);

    if (!file.is_open())
    {
        return false;
    }

    Tile tile;

    while (file >> tile.x >> tile.y >> tile.textureIndex)
    {
        worldWalls.push_back(tile);
    }

    return true;
}