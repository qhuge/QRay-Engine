#pragma once
#include <string>

float CalcColorMult(float distance);

float CalcAmbientMult(float distanceFromCorner);

bool FloatEquals(float a, float b);

int clampInt(int a, int max);