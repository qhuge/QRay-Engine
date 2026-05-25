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

    float movementAngle = -angleOffset;

    float moveSpeedNow = moveSpeed;

    if (KeyDown(VK_SHIFT))
    {
        moveSpeedNow = runSpeed;
    }

    float angleRad = angleOffset * M_PI / 180.0f;

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

    playerX += moveX * moveSpeedNow;
    playerY += moveY * moveSpeedNow;

    // =========================================
    // ROTATION
    // =========================================

    if (KeyDown(VK_RIGHT))
    {
        angleOffset += angleSpeed;
    }

    if (KeyDown(VK_LEFT))
    {
        angleOffset -= angleSpeed;
    }

    if (angleOffset >= 270) { angleOffset = -90; }
    else if (angleOffset <= -270) { angleOffset = 90; }

    // =========================================
    // FLOOR + CEILING
    // =========================================

    for (int y = 0; y < WINDOW_HEIGHT / 2; y++)
    {
        for (int x = 0; x < WINDOW_WIDTH; x++)
        {
            PutPixel(win32, x, y, 0x00787878);
        }
    }

    for (int y = WINDOW_HEIGHT / 2; y < WINDOW_HEIGHT; y++)
    {
        for (int x = 0; x < WINDOW_WIDTH; x++)
        {
            PutPixel(win32, x, y, 0x003C3C3C);
        }
    }

    // =========================================
    // RAYCAST RENDERING
    // =========================================



    float currentX = 0;
    for (int k = ((-(FOV / 2.0f)) + angleOffset) * scale; k <= ((FOV / 2.0f) + angleOffset) * scale; k++) {
        float i = k / scale;
        RayHit hit = CastRay(playerX, playerY, i);

        if (hit.distance > 0.0f)
        {
            float correctedDistance = cosf((i - angleOffset) * M_PI / 180.0f) * hit.distance;

            if (correctedDistance < 0.01f)
            {
                correctedDistance = 0.01f;
            }

            float projectionPlaneDistance = (WINDOW_WIDTH / 2.0f) / tanf((FOV * 0.5f) * M_PI / 180.0f);
            float wallHeight = projectionPlaneDistance / correctedDistance;

            int mirrorY = 1;
            for (int p = (WINDOW_HEIGHT / 2); p <= ((wallHeight / 2) + (WINDOW_HEIGHT / 2)); p++) {

                float brightness = 255.0f * CalcColorMult(correctedDistance);

                brightness *= hit.ambient;

                /*std::string msg = std::to_string(brightness) + "\n";
                OutputDebugStringA(msg.c_str());*/

                if (brightness > 255.0f)
                    brightness = 255.0f;

                if (brightness < 0.0f)
                    brightness = 0.0f;

                uint8_t c = (uint8_t)brightness;

                uint32_t color =
                    (c << 16) |
                    (c << 8) |
                    c;
                PutPixel(win32, std::round(currentX), p, color);
                PutPixel(win32, std::round(currentX), ((WINDOW_HEIGHT / 2) - mirrorY), color);

                mirrorY++;
            }
        }

        currentX += WINDOW_WIDTH / (FOV * scale);
    }
}
