#pragma once
#include <string>
#include "entity_rendering.hpp"

float CalcColorMult(float distance);

float CalcAmbientMult(float distanceFromCorner);

bool FloatEquals(float a, float b);

int clampInt(int a, int max);

bool compareDistance(const EntityToRender& a, const EntityToRender& b);

bool PlayerBetweenDoor(const Tile& doorTile1, const Tile& doorTile2);