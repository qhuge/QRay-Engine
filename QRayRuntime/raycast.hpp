#pragma once

struct RayHit
{
    float distance;
    float textureX;
    float ambient;
    int textureIndex;
    int tileIndex;

    RayHit(float d = -1.0f, float tx = 0.0f, float amb = 0.0f, int ti = 0, int tilei = 0) : distance(d), textureX(tx), ambient(amb), textureIndex(ti), tileIndex(tilei)
    {
    }
};

RayHit CastRay(float x, float y, float angle);