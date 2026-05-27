#define _USE_MATH_DEFINES

#include "renderer.h"
#include <cmath>
#include "raycast.h"
#include "config.h"
#include "helperFunctions.h"
#include "window_win32.h"

void Render(Win32State& win32)
{
    ClearScreen(win32, 0x00000000);

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

    cfg.playerX += moveX * moveSpeedNow * deltaTime;
    cfg.playerY += moveY * moveSpeedNow * deltaTime;

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

    for (int y = 0; y < cfg.WINDOW_HEIGHT / 2; y++)
    {
        for (int x = 0; x < cfg.WINDOW_WIDTH; x++)
        {
            PutPixel(win32, x, y, 0x00787878);
        }
    }

    for (int y = cfg.WINDOW_HEIGHT / 2; y < cfg.WINDOW_HEIGHT; y++)
    {
        for (int x = 0; x < cfg.WINDOW_WIDTH; x++)
        {
            PutPixel(win32, x, y, 0x003C3C3C);
        }
    }

    // =========================================
    // RAYCAST RENDERING
    // =========================================

    
    float projectionPlaneDistance = (cfg.WINDOW_WIDTH / 2.0f) / tanf((cfg.FOV * 0.5f) * M_PI / 180.0f);
    for (int x = 0; x < cfg.WINDOW_WIDTH; x++) {
        float rayAngle = cfg.angleOffset - (cfg.FOV / 2.0f) + ((float)x / cfg.WINDOW_WIDTH) * cfg.FOV;

        RayHit hit = CastRay(cfg.playerX, cfg.playerY, rayAngle);

        if (hit.distance > 0.0f)
        {
            float correctedDistance = cosf((rayAngle - cfg.angleOffset) * M_PI / 180.0f) * hit.distance;

            if (correctedDistance < 0.01f)
            {
                correctedDistance = 0.01f;
            }

            float projectionPlaneDistance = (cfg.WINDOW_WIDTH / 2.0f) / tanf((cfg.FOV * 0.5f) * M_PI / 180.0f);
            float wallHeight = projectionPlaneDistance / correctedDistance;

            int textureIndex = hit.textureIndex;
            bool isValidTexture = true;

            Texture* tex = nullptr;

            if (textureIndex >= 0 && textureIndex < gTextures.size())
            {
                tex = &gTextures[textureIndex];
            }

            

            int startY = cfg.WINDOW_HEIGHT / 2 - (wallHeight / 2);

            int endY = startY + (int)wallHeight;

            for (int p = startY; p < endY; p++)
            {
                if (p < 0 || p > cfg.WINDOW_HEIGHT) {
                    continue;
                }

                float brightness = CalcColorMult(correctedDistance) * hit.ambient;

                float r;
                float g;
                float b;

                if (!tex) {
                    r = 1.0;
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

                PutPixel(win32, x, p, color);
            }
        }
    }
}
