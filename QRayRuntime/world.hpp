#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include "types.hpp"
#include "config.hpp"

enum class Direction
{
    Up,
    Right,
    Down,
    Left
};

enum class Axis
{
    Horizontal,
    Vertical
};

struct Door
{
    float open = 0.0f; //0 = closed, 1 = open
    Axis dir;
    Direction renderedFace;
    bool targetOpen = false;
    float openTimer = 0.0f;
    int indexOfOtherDoorTile;
    int tag;
};

struct Tile
{
    int x;
    int y;
    int textureIndex;

    bool isDoor = false;
    Door door;
};

struct Entity
{
    float x;
    float y;
    uint16_t textureIndex;
    int entityTypeIndex; // this is a index in gEntityTypes, calculated at game start based on textureIndex, textureAmount and entityAmount.
    bool active = true;
    int health;
    float targetX;//used for movement
    float targetY;//used for movement
    int frameIndex = 0; //used for anims
    int maxFrameIndex; //used for anims
    bool animate = false; //use animation?
    float animationDuration;
    float maxAnimationDuration;
};

bool TileMatches(const Tile& tile, int x, int y);

int FindWall(int x, int y);

extern std::vector<Tile> worldWalls;
extern int worldGrid[64][64];
extern std::vector<Entity> worldEntities;

bool LoadWorld(const char* filename);

Texture LoadQRayAsset(std::string path);

EntityType LoadQRayEntity(std::string path);