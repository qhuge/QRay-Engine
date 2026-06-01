#include "world.hpp"
#include <fstream>
#include <sstream>
#include <string>

//const std::vector<Tile> worldWalls =
//{
//    {100,104, 0},
//    {101,105, 0},
//    {99,105, 0},
//    {99,103, 0},
//    {101,103, 0},
//};

std::vector<Tile> worldWalls;

std::vector<Entity> worldEntities;

bool TileMatches(const Tile& tile, int x, int y)
{
    return tile.x == x && tile.y == y;
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
    worldEntities.clear();

    std::ifstream file(filename);

    if (!file.is_open())
        return false;

    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        std::stringstream ss(line);

        char type;
        ss >> type;

        switch (type)
        {
        case 'T':
        {
            Tile tile;

            ss >> tile.x >> tile.y >> tile.textureIndex;

            worldWalls.push_back(tile);
            break;
        }

        case 'E':
        {
            Entity entity;

            ss >> entity.x >> entity.y >> entity.textureIndex;

            entity.x += 0.5f;
            entity.y += 0.5f;

            worldEntities.push_back(entity);
            break;
        }

        //TODO spawn based on map file:
        /*case 'S':
        {
            ss >> spawnX >> spawnY;
            break;
        }*/

        default:
            // Unknown line type
            break;
        }
    }

    return true;
}

Texture LoadQRayAsset(std::string path)
{
    std::ifstream file(path, std::ios::binary);

    QRayTextureAssetHeader header;
    file.read((char*)&header, sizeof(header));

    Texture tex;
    tex.width = header.width;
    tex.height = header.height;

    size_t size = header.width * header.height * 4;
    tex.pixels.resize(size);

    file.read((char*)tex.pixels.data(), size);

    return tex;
}

EntityType LoadQRayEntity(std::string path)
{
    std::ifstream file(path, std::ios::binary);

    EntityType entityT;

    file.read((char*)&entityT, sizeof(entityT));

    file.close();

    return entityT;
}