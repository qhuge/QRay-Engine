#define _USE_MATH_DEFINES

#include "helperFunctions.hpp"
#include <cmath>
#include "config.hpp"
#include "entity_rendering.hpp"

float CalcColorMult(float distance)
{
    float mult = 1.0f / (1.0f + distance * cfg.lightDecay);

    if (mult > 1.0f)
        mult = 1.0f;

    if (mult < 0.1f)
        mult = 0.1f;

    return mult;
}
float CalcAmbientMult(float distanceFromCorner)
{
    float mult = 1.0f * distanceFromCorner + 0.5f;

    if (mult > 1.0f)
        mult = 1.0f;

    if (mult < 0.5f)
        mult = 0.5f;

    return mult;
}
bool FloatEquals(float a, float b)
{
    return std::abs(a - b) < 0.001f;
}

int clampInt(int a, int max) {
    if (a < 0) {
        a = 0;
    }
    else if (a > max) {
        a = max;
    }
    return a;
}

bool compareDistance(const EntityToRender& a, const EntityToRender& b)
{
    return a.d > b.d;
}

bool IsNearTile(const Tile& tile, float margin)
{
    return cfg.playerX > tile.x - margin &&
        cfg.playerX < tile.x + 1.0f + margin &&
        cfg.playerY > tile.y - margin &&
        cfg.playerY < tile.y + 1.0f + margin;
}

bool PlayerBetweenDoor(const Tile& doorTile1, const Tile& doorTile2)
{
    return IsNearTile(doorTile1, 1.5f) || IsNearTile(doorTile2, 1.5f);
}

bool isWallAt(int x, int y)
{
    int wall = FindWall(x, y);

    if (wall != -1) {
        Tile t = worldWalls[wall];
        if (!t.isDoor || (t.isDoor && t.door.open <= 0.9f)) {
            return true;
        }
    }
    
    return false;
}

//Player radius
const float rP = 0.3f;
//entity radius (note that later could be assigned entity by entity bases so each one could have variable radius, i think it would be cool :) )
const float rE = 0.3f;
const float rESq = (rP + rE) * (rP + rE);

bool CanMoveTo(float x, float y)
{
    //calculate a bunch of values
    int cellX = (int)floorf(x);
    int cellY = (int)floorf(y);

    int left = (int)floorf(x - rP);
    int right = (int)floorf(x + rP);
    int top = (int)floorf(y - rP);
    int bottom = (int)floorf(y + rP);

    //check walls
    if ((isWallAt(cellX + 1, cellY) && cellX != right) ||
        (isWallAt(cellX - 1, cellY) && cellX != left) ||
        (isWallAt(cellX, cellY + 1) && cellY != bottom) ||
        (isWallAt(cellX, cellY - 1) && cellY != top) ||
        (isWallAt(cellX + 1, cellY + 1) && (cellY != bottom && cellX != right)) ||
        (isWallAt(cellX + 1, cellY - 1) && (cellY != top && cellX != right)) ||
        (isWallAt(cellX - 1, cellY - 1) && (cellY != top && cellX != left)) ||
        (isWallAt(cellX - 1, cellY + 1) && (cellY != bottom && cellX != left))

        ) {
        return false;
    }

    //check entities
    for (auto& entity : worldEntities) {
        //ignore pickup entities and unactive ones
        if (gEntityTypes[entity.entityTypeIndex].tag != 0 || !entity.active) {
            continue;
        }

        //calulate the difference in each coordinate
        float dx = x - entity.x;
        float dy = y - entity.y;

        //compare distances squared
        float distanceSquared = (dx * dx) + (dy * dy);

        if (distanceSquared <= rESq) {
            return false;
        }
    }

    return true;
}

bool hasTag(int tag) {
    return gPlayerTags.count(tag) == 1;
}