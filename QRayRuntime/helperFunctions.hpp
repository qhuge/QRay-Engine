#pragma once
#include <string>
#include "entity_rendering.hpp"

float CalcColorMult(float distance);

float CalcAmbientMult(float distanceFromCorner);

bool FloatEquals(float a, float b);

int clampInt(int a, int max);

bool compareDistance(const EntityToRender& a, const EntityToRender& b);

bool IsNearTile(const Tile& tile, float margin);

bool PlayerBetweenDoor(const Tile& doorTile1, const Tile& doorTile2);

bool isWallAt(int x, int y);

bool CanMoveTo(float x, float y);

bool hasTag(int tag);