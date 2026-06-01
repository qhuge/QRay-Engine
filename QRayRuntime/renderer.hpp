#pragma once
#include <vector>
#include "world.hpp"

struct Win32State;

struct Framebuffer
{
    uint32_t* pixels;

    int width;
    int height;
};

extern std::vector<float> gDepthBuffer;

void Render(Framebuffer framebuffer);

struct EntityToRender
{
    Entity* e;
    float d;
    float a;
};