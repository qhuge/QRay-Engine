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

bool IsWallAt(float x, float y)
{
    int wall = FindWall((int)floorf(x), (int)floorf(y));

    if (wall == -1) {
        return false;
    }
    else {
        Tile t = worldWalls[wall];
        if (!t.isDoor) {
            return true;
        }
        if (t.door.open >= 0.9f) {
            return false;
        }
    }
    return true;
}

bool CanMoveTo(float x, float y)
{
    const float r = 0.2f;

    return
        !IsWallAt(x - r, y - r) &&
        !IsWallAt(x + r, y - r) &&
        !IsWallAt(x - r, y + r) &&
        !IsWallAt(x + r, y + r);
}