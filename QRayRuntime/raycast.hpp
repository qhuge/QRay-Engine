#pragma once

struct RayHit
{
    float distance;
    float textureX;
    float ambient;
    int textureIndex;

    RayHit(float d = -1.0f, float tx = 0.0f, float amb = 0.0f, int ti = 0) : distance(d), textureX(tx), ambient(amb), textureIndex(ti)
    {
    }
};

RayHit CastRay(float x, float y, float angle);