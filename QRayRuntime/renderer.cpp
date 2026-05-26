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

    cfg.playerX += moveX * moveSpeedNow;
    cfg.playerY += moveY * moveSpeedNow;

    // =========================================
    // ROTATION
    // =========================================

    if (KeyDown(VK_RIGHT))
    {
        cfg.angleOffset += cfg.angleSpeed;
    }

    if (KeyDown(VK_LEFT))
    {
        cfg.angleOffset -= cfg.angleSpeed;
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



    float currentX = 0;
    for (int k = ((-(cfg.FOV / 2.0f)) + cfg.angleOffset) * cfg.scale; k <= ((cfg.FOV / 2.0f) + cfg.angleOffset) * cfg.scale; k++) {
        float i = k / cfg.scale;
        RayHit hit = CastRay(cfg.playerX, cfg.playerY, i);

        if (hit.distance > 0.0f)
        {
            float correctedDistance = cosf((i - cfg.angleOffset) * M_PI / 180.0f) * hit.distance;

            if (correctedDistance < 0.01f)
            {
                correctedDistance = 0.01f;
            }

            float projectionPlaneDistance = (cfg.WINDOW_WIDTH / 2.0f) / tanf((cfg.FOV * 0.5f) * M_PI / 180.0f);
            float wallHeight = projectionPlaneDistance / correctedDistance;


            //fixes low fps near walls
            if (wallHeight > cfg.WINDOW_HEIGHT) {
                wallHeight = cfg.WINDOW_HEIGHT;
            }

            int mirrorY = 1;
            for (int p = (cfg.WINDOW_HEIGHT / 2); p <= ((wallHeight / 2) + (cfg.WINDOW_HEIGHT / 2)); p++) {

                float brightness = 255.0f * CalcColorMult(correctedDistance);

                brightness *= hit.ambient;

                /*std::string msg = std::to_string(brightness) + "\n";
                OutputDebugStringA(msg.c_str());*/

                if (brightness > 255.0f)
                    brightness = 255.0f;

                if (brightness < 0.0f)
                    brightness = 0.0f;

                uint8_t c = (uint8_t)brightness;

                uint32_t color = (c << 16) | (c << 8) | c;
                PutPixel(win32, std::round(currentX), p, color);
                PutPixel(win32, std::round(currentX), ((cfg.WINDOW_HEIGHT / 2) - mirrorY), color);

                mirrorY++;
            }
        }

        currentX += cfg.WINDOW_WIDTH / (cfg.FOV * cfg.scale);
    }
}
