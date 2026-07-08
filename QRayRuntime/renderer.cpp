#define _USE_MATH_DEFINES
#define NOMINMAX


#include "renderer.hpp"
#include <cmath>
#include <algorithm>
#include "raycast.hpp"
#include "config.hpp"
#include "helperFunctions.hpp"
#include "window_win32.hpp"
#include "world.hpp"
#include "types.hpp"
#include "entity_rendering.hpp"

std::vector<float> gDepthBuffer;

void Render(Framebuffer framebuffer)
{

    //FLOOR + CEILING
    for (int y = 0; y < framebuffer.height / 2; y++)
    {
        uint32_t* row = framebuffer.pixels + y * framebuffer.width;

        for (int x = 0; x < framebuffer.width; x++)
        {
            row[x] = cfg.ceilingColor;
        }
    }

    for (int y = framebuffer.height / 2; y < framebuffer.height; y++)
    {
        uint32_t* row = framebuffer.pixels + y * framebuffer.width;

        for (int x = 0; x < framebuffer.width; x++)
        {
            row[x] = cfg.floorColor;
        }
    }

    //RENDER
    
    //reset the depth buffer so its ready for a new frame
    gDepthBuffer.clear();

    float projectionPlaneDistance = (framebuffer.width / 2.0f) / tanf((cfg.FOV * 0.5f) * M_PI / 180.0f);
    for (int x = 0; x < framebuffer.width; x++) {
        float rayAngle = cfg.angleOffset - (cfg.FOV / 2.0f) + ((float)x / framebuffer.width) * cfg.FOV;

        RayHit hit = CastRay(cfg.playerX, cfg.playerY, rayAngle);

        //if the key E is pressed
        if (GetAsyncKeyState('E') & 0x8000)
        {
            if (hit.distance > 0.0f && hit.distance < 1.5f)
            {
                Tile& tile = worldWalls[hit.tileIndex];

                if (tile.isDoor && (tile.door.tag == 0 || hasTag(tile.door.tag)))
                {

                    tile.door.targetOpen = true;
                    tile.door.openTimer = 5.0f;

                    worldWalls[tile.door.indexOfOtherDoorTile].door.targetOpen = true;
                    worldWalls[tile.door.indexOfOtherDoorTile].door.openTimer = 5.0f;
                }
            }
        }

        //if we actually hit something
        if (hit.distance > 0.0f)
        {
            //calucate the perpendicular distance. TODO: this (rayAngle - cfg.angleOffset) might be incorrect
            float correctedDistance = cosf((rayAngle - cfg.angleOffset) * M_PI / 180.0f) * hit.distance;
             
            if (correctedDistance < 0.01f)
            {
                correctedDistance = 0.01f;
            }

            //add the distance to the depth buffer
            gDepthBuffer.push_back(correctedDistance);

            float wallHeight = projectionPlaneDistance / correctedDistance;

            int textureIndex = hit.textureIndex;

            Texture* tex = nullptr;

            if (textureIndex >= 0 && textureIndex < gTextures.size())
            {
                tex = &gTextures[textureIndex];
            }

            int startY = framebuffer.height / 2 - (wallHeight / 2);

            int endY = startY + (int)wallHeight;

            //calculate the brightness
            float brightness = CalcColorMult(correctedDistance) * hit.ambient;

            //int multiplication is faster than float
            int light = (int)(brightness * 255.0f);

            int texX = (int)(hit.textureX * tex->width);

            //calculate a 16.16 step avoid dividing
            int step = (tex->height << 16) / wallHeight;
            int clippedStart = std::max(startY, 0);
            int texPos = (clippedStart - startY) * step;

            for (int p = startY; p < endY; p++)
            {
                if (p <= 0 || p >= framebuffer.height) {
                    continue;
                }

                uint32_t pixel;

                if (!tex) {
                    //if theres an invalid texture then display purple color
                    pixel = 0x00FF00FF;
                }
                else {
                    //calculate the texture y coordinate
                    int texY = texPos >> 16;

                    //ensure that the texture coordinates are within bounds
                    texX = clampInt(texX, tex->width - 1);
                    texY = clampInt(texY, tex->height - 1);

                    //get the index of the color at texX, texY.
                    int index = texY * tex->width + texX;

                    //get the pixel at that specific index
                    pixel = tex->pixels[0][index];
                }
                
                //increment the texPos
                texPos += step;

                //get raw pixel values
                uint8_t r = pixel & 0xFF;
                uint8_t g = (pixel >> 8) & 0xFF;
                uint8_t b = (pixel >> 16) & 0xFF;

                //>>8 is faster than dividing by 255
                r = (r * light) >> 8;
                g = (g * light) >> 8;
                b = (b * light) >> 8;

                //combine the channels back
                uint32_t color = (r << 16) | (g << 8) | b;

                //writing directly to the framebuffer is probably faster
                framebuffer.pixels[p * framebuffer.width + x] = color;
            }
        }
        else {
            //if we didnt hit anything, then add -1 to the depth buffer
            gDepthBuffer.push_back(-1.0f);
        }
    }

    //make a vector of the entities. we can save the distance and angle to it so they dont have to be calculated again later.
    std::vector<EntityToRender> entToRender;
    for (int i = 0; i < worldEntities.size(); i++) {
        Entity& e = worldEntities[i];

        //Dont render if not active
        if (!e.active) {
            continue;
        }

        //relative positions
        float dx = e.x - cfg.playerX;
        float dy = e.y - cfg.playerY;

        float distance = sqrtf(dx * dx + dy * dy);

        // Skip extremely close entities and far away ones
        if (distance <= 0.01f || distance >= cfg.renderDistance)
        {
            continue;
        }

        //angles
        float angleToEntity = atan2f(dy, dx) * 180.0f / M_PI;

        float relativeAngle = angleToEntity - cfg.angleOffset;

        while (relativeAngle > 180.0f)
            relativeAngle -= 360.0f;

        while (relativeAngle < -180.0f)
            relativeAngle += 360.0f;

        // Outside FOV
        if (fabs(relativeAngle) > (cfg.FOV * 0.5f))
        {
            continue;
        }

        //correction
        float correctedDistance = cosf(relativeAngle * M_PI / 180.0f) * distance;

        if (correctedDistance <= 0.01f || correctedDistance > cfg.renderDistance)
        {
            continue;
        }

        EntityToRender etr =
        {
            &worldEntities[i],
            correctedDistance,
            relativeAngle
        };
        entToRender.push_back(etr);
    }

    //sort entities from closest to farthest away
    std::sort(entToRender.begin(), entToRender.end(), compareDistance);

    //render entities
    for(int i = 0; i < entToRender.size(); i++)
    {
        //get values precalculated from the array
        Entity* e = entToRender[i].e;
        float correctedDistance = entToRender[i].d;
        float relativeAngle = entToRender[i].a;

        //projection
        float screenXFloat = ((relativeAngle + (cfg.FOV * 0.5f)) / cfg.FOV) * framebuffer.width;

        int screenX = (int)screenXFloat;

        int spriteSize = (int)(projectionPlaneDistance / correctedDistance);

        if (spriteSize <= 0)
        {
            continue;
        }

        int halfSprite = spriteSize / 2;

        int spriteLeft = screenX - halfSprite;

        int spriteRight = screenX + halfSprite;

        int spriteTop = (framebuffer.height / 2) - halfSprite;

        int spriteBottom = spriteTop + spriteSize;

        //offscreen culling
        if (spriteRight < 0 || spriteLeft >= framebuffer.width)
        {
            continue;
        }

        //clamp draw-area
        int startX = std::max(0, spriteLeft);

        int endX = std::min(framebuffer.width, spriteRight);

        int startY = std::max(0, spriteTop);

        int endY = std::min(framebuffer.width, spriteBottom);

        //texture
        Texture* tex = nullptr;

        if (e->textureIndex >= 0 && e->textureIndex < gTextures.size())
        {
            tex = &gTextures[e->textureIndex];
        }

        //calculate the brightness
        float brightness = CalcColorMult(correctedDistance);

        //int multiplication is faster than float
        int light = (int)(brightness * 255.0f);

        //actual render
        for (int x = startX; x < endX; x++)
        {
            //depth test once per column
            if (!(correctedDistance < gDepthBuffer[x] || gDepthBuffer[x] == -1))
            {
                continue;
            }

            int textureX = 0;

            if (tex)
            {
                textureX = ((x - spriteLeft) * tex->width) / spriteSize;

                if (textureX < 0)
                    textureX = 0;

                if (textureX >= tex->width)
                    textureX = tex->width - 1;
            }

            //rows
            for (int y = startY; y < endY; y++)
            {

                if (y < 0 || y >= framebuffer.height) {
                    continue;
                }

                uint32_t framebufferColor = 0x00FF00FF;

                if (tex)
                {
                    int textureY = ((y - spriteTop) * tex->height) / spriteSize;

                    if (textureY < 0)
                        textureY = 0;

                    if (textureY >= tex->height)
                        textureY = tex->height - 1;

                    //get the index of the color at textureX, textureY.
                    int index = textureY * tex->width + textureX;

                    //get the color (use frameIndex, that way the animations actually work!!!!!)
                    uint32_t color = tex->pixels[e->frameIndex][index];

                    //get the alpha to see if its transparent
                    uint8_t a = (color >> 24) & 0xFF;

                    //do not render the pixel if its transparent
                    if (a == 0)
                    {
                        continue;
                    }

                    //get raw pixel values
                    uint8_t r = color & 0xFF;
                    uint8_t g = (color >> 8) & 0xFF;
                    uint8_t b = (color >> 16) & 0xFF;

                    //>>8 is faster than dividing by 255
                    r = (r * light) >> 8;
                    g = (g * light) >> 8;
                    b = (b * light) >> 8;

                    //combine the channels back
                    framebufferColor = (r << 16) | (g << 8) | b;
                }

                //write to the framebuffer
                framebuffer.pixels[y * framebuffer.width + x] = framebufferColor;
            }
        }
    }
}
