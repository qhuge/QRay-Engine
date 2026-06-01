#define _USE_MATH_DEFINES

#include "helperFunctions.hpp"
#include <cmath>
#include "config.hpp"
#include "renderer.hpp"

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