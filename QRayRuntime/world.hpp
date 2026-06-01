#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include "types.hpp"
#include "config.hpp"



struct Tile
{
    int x;
    int y;
    int textureIndex;
};

struct Entity
{
    float x;
    float y;
    uint16_t textureIndex;
    bool active = true;
};

bool TileMatches(const Tile& tile, int x, int y);

int FindWall(int x, int y);

extern std::vector<Tile> worldWalls;
extern std::vector<Entity> worldEntities;

bool LoadWorld(const char* filename);

Texture LoadQRayAsset(std::string path);
EntityType LoadQRayEntity(std::string path);