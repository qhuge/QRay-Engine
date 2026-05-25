#include "world.h"

const std::vector<Tile> worldWalls =
{
    {100,104},
    {101,105},
    {99,105},
    {99,103},
    {101,103},
};

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