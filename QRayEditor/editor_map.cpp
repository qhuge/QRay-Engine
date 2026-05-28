#include "editor_map.hpp"

#include <fstream>
#include <string>

std::vector<Tile> worldMap;

void SaveMap()
{
    /*std::ofstream file("map.txt");

    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            file << worldMap[y][x];
        }

        file << "\n";
    }*/
}

void LoadMap()
{
    /*std::ifstream file("map.txt");

    std::string line;

    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        std::getline(file, line);

        for (int x = 0; x < MAP_WIDTH; x++)
        {
            worldMap[y][x] = line[x] - '0';
        }
    }*/
}

int FindWall(int x, int y)
{
    for (int i = 0; i < worldMap.size(); i++)
    {
        if (worldMap[i].x == x &&
            worldMap[i].y == y)
        {
            return i;
        }
    }
    return -1;
}