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

const float playerRadius = 0.2f;

std::vector<float> gDepthBuffer;

void Render(Framebuffer framebuffer)
{
    ClearScreen(framebuffer, 0x00000000);

    // =========================================
    // INPUT
    // =========================================

    float movementAngle = -cfg.angleOffset;

    float moveSpeedNow = cfg.moveSpeed;

    if (KeyDown(VK_SHIFT))
    {
        moveSpeedNow = cfg.runSpeed;
    }

    float angleRad = cfg.angleOffset * M_PI / 180.0f;

    float forwardX = cosf(angleRad);
    float forwardY = sinf(angleRad);

    float rightX = cosf(angleRad + M_PI * 0.5f);
    float rightY = sinf(angleRad + M_PI * 0.5f);

    float moveX = 0.0f;
    float moveY = 0.0f;

    // Forward
    if (KeyDown('W'))
    {
        moveX += forwardX;
        moveY += forwardY;
    }

    // Backward
    if (KeyDown('S'))
    {
        moveX -= forwardX;
        moveY -= forwardY;
    }

    // Left
    if (KeyDown('A'))
    {
        moveX -= rightX;
        moveY -= rightY;
    }

    // Right
    if (KeyDown('D'))
    {
        moveX += rightX;
        moveY += rightY;
    }

    float length = sqrtf(moveX * moveX + moveY * moveY);

    if (length > 0.0f)
    {
        moveX /= length;
        moveY /= length;
    }

    float newX = cfg.playerX + moveX * moveSpeedNow * deltaTime;
    float newY = cfg.playerY + moveY * moveSpeedNow * deltaTime;

    if (CanMoveTo(newX, cfg.playerY))
    {
        cfg.playerX = newX;
    }

    if (CanMoveTo(cfg.playerX, newY))
    {
        cfg.playerY = newY;
    }

    // =========================================
    // ROTATION
    // =========================================

    if (KeyDown(VK_RIGHT))
    {
        cfg.angleOffset += cfg.angleSpeed * deltaTime;
    }

    if (KeyDown(VK_LEFT))
    {
        cfg.angleOffset -= cfg.angleSpeed * deltaTime;
    }

    if (cfg.angleOffset >= 270) { cfg.angleOffset = -90; }
    else if (cfg.angleOffset <= -270) { cfg.angleOffset = 90; }

    // =========================================
    // FLOOR + CEILING
    // =========================================

    for (int y = 0; y < framebuffer.height / 2; y++)
    {
        for (int x = 0; x < framebuffer.width; x++)
        {
            PutPixel(framebuffer, x, y, 0x00787878);
        }
    }

    for (int y = framebuffer.height / 2; y < framebuffer.height; y++)
    {
        for (int x = 0; x < framebuffer.width; x++)
        {
            PutPixel(framebuffer, x, y, 0x003C3C3C);
        }
    }

    //RENDER
    
    gDepthBuffer.clear();

    float projectionPlaneDistance = (framebuffer.width / 2.0f) / tanf((cfg.FOV * 0.5f) * M_PI / 180.0f);
    for (int x = 0; x < framebuffer.width; x++) {
        float rayAngle = cfg.angleOffset - (cfg.FOV / 2.0f) + ((float)x / framebuffer.width) * cfg.FOV;

        RayHit hit = CastRay(cfg.playerX, cfg.playerY, rayAngle);

        if (GetAsyncKeyState('E') & 0x8000)
        {
            if (hit.distance > 0.0f && hit.distance < 1.5f)
            {
                Tile& tile = worldWalls[hit.tileIndex];

                if (tile.isDoor)
                {
                    tile.door.targetOpen = true;
                    tile.door.openTimer = 5.0f;

                    worldWalls[tile.door.indexOfOtherDoorTile].door.targetOpen = true;
                    worldWalls[tile.door.indexOfOtherDoorTile].door.openTimer = 5.0f;
                }
            }
        }


        if (hit.distance > 0.0f)
        {
            float correctedDistance = cosf((rayAngle - cfg.angleOffset) * M_PI / 180.0f) * hit.distance;

            if (correctedDistance < 0.01f)
            {
                correctedDistance = 0.01f;
            }

            gDepthBuffer.push_back(correctedDistance);

            float wallHeight = projectionPlaneDistance / correctedDistance;

            int textureIndex = hit.textureIndex;
            bool isValidTexture = true;

            Texture* tex = nullptr;

            if (textureIndex >= 0 && textureIndex < gTextures.size())
            {
                tex = &gTextures[textureIndex];
            }

            int startY = framebuffer.height / 2 - (wallHeight / 2);

            int endY = startY + (int)wallHeight;

            for (int p = startY; p < endY; p++)
            {
                if (p < 0 || p > framebuffer.height) {
                    continue;
                }

                float brightness = CalcColorMult(correctedDistance) * hit.ambient;

                float r;
                float g;
                float b;

                if (!tex) {
                    r = 1.0f;
                    g = 0.0f;
                    b = 1.0f;
                }
                else {
                    int texX = (int)(hit.textureX * tex->width);

                    int texY = ((p - startY) * tex->height) / (int)wallHeight;

                    texX = clampInt(texX, tex->width - 1);
                    texY = clampInt(texY, tex->height - 1);

                    int index = (texY * tex->width + texX) * 4;

                    r = tex->pixels[index + 0] / 255.0f;
                    g = tex->pixels[index + 1] / 255.0f;
                    b = tex->pixels[index + 2] / 255.0f;
                }
                

                r *= brightness;
                g *= brightness;
                b *= brightness;

                uint32_t color =
                    ((uint8_t)(r * 255) << 16) |
                    ((uint8_t)(g * 255) << 8) |
                    ((uint8_t)(b * 255));

                PutPixel(framebuffer, x, p, color);
            }
        }
        else {
            gDepthBuffer.push_back(-1.0f);
        }
    }


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

        // Skip extremely close entities
        if (distance <= 0.01f)
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

        //actual render
        for (int x = startX; x < endX; x++)
        {
            // Depth test once per column
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
                uint32_t color = 0xFF00FF;

                if (tex)
                {
                    int textureY = ((y - spriteTop) * tex->height) / spriteSize;

                    if (textureY < 0)
                        textureY = 0;

                    if (textureY >= tex->height)
                        textureY = tex->height - 1;

                    int index = (textureY * tex->width + textureX) * 4;

                    unsigned char r = tex->pixels[index + 0];

                    unsigned char g = tex->pixels[index + 1];

                    unsigned char b = tex->pixels[index + 2];

                    unsigned char a = tex->pixels[index + 3];

                    // Transparent pixel
                    if (a == 0)
                    {
                        continue;
                    }

                    color = (r << 16) | (g << 8) | b;
                }

                PutPixel(framebuffer, x, y, color);
            }
        }
    }
}
