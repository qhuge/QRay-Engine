#pragma once

struct RayHit
{
    float distance;
    float textureX;
    float ambient;

    RayHit(float d = -1.0f, float tx = 0.0f, float amb = 0.0f) : distance(d), textureX(tx), ambient(amb)
    {
    }
};

RayHit CastRay(float x, float y, float angle);