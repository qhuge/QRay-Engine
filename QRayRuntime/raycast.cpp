#define _USE_MATH_DEFINES

#include "raycast.h"
#include "config.h"
#include "helperFunctions.h"
#include "world.h"
#include <cmath>

RayHit CastRay(float posX, float posY, float angleDeg)
{
    // =========================================================
    // Convert angle to direction vector
    // =========================================================

    float angleRad = angleDeg * M_PI / 180.0f;

    float rayDirX = std::cos(angleRad);
    float rayDirY = std::sin(angleRad);

    // Prevent divide-by-zero
    if (std::abs(rayDirX) < 0.0001f)
        rayDirX = 0.0001f;

    if (std::abs(rayDirY) < 0.0001f)
        rayDirY = 0.0001f;

    // =========================================================
    // Current map tile
    // =========================================================

    int mapX = (int)std::floor(posX);
    int mapY = (int)std::floor(posY);

    // =========================================================
    // Distance ray travels to cross one tile
    // =========================================================

    float deltaDistX = std::abs(1.0f / rayDirX);
    float deltaDistY = std::abs(1.0f / rayDirY);

    // =========================================================
    // Step direction
    // =========================================================

    int stepX;
    int stepY;

    float sideDistX;
    float sideDistY;

    if (rayDirX < 0.0f)
    {
        stepX = -1;
        sideDistX = (posX - mapX) * deltaDistX;
    }
    else
    {
        stepX = 1;
        sideDistX = (mapX + 1.0f - posX) * deltaDistX;
    }

    if (rayDirY < 0.0f)
    {
        stepY = -1;
        sideDistY = (posY - mapY) * deltaDistY;
    }
    else
    {
        stepY = 1;
        sideDistY = (mapY + 1.0f - posY) * deltaDistY;
    }

    // =========================================================
    // DDA
    // =========================================================

    bool hit = false;

    int side = 0;

    // side:
    // 0 = vertical wall
    // 1 = horizontal wall

    for (int i = 0; i < cfg.renderDistance; i++)
    {
        int prevMapX = mapX;
        int prevMapY = mapY;

        // -----------------------------------------------------
        // Step to next tile
        // -----------------------------------------------------

        if (sideDistX < sideDistY)
        {
            sideDistX += deltaDistX;
            mapX += stepX;
            side = 0;
        }
        else
        {
            sideDistY += deltaDistY;
            mapY += stepY;
            side = 1;
        }

        int WallIndex = FindWall(mapX, mapY);

        if (WallIndex == -1)
            continue;

        hit = true;

        float perpWallDist;

        if (side == 0)
        {
            perpWallDist =
                (mapX - posX + (1 - stepX) * 0.5f)
                / rayDirX;
        }
        else
        {
            perpWallDist =
                (mapY - posY + (1 - stepY) * 0.5f)
                / rayDirY;
        }

        float wallHit;

        if (side == 0)
        {
            wallHit =
                posY + perpWallDist * rayDirY;
        }
        else
        {
            wallHit =
                posX + perpWallDist * rayDirX;
        }

        // Fractional part
        float textureX =
            wallHit - std::floor(wallHit);

        //Ambient occlusion

        float ambient = 1.0f;

        if (side == 0) {
            if (rayDirX >= 0) {
                if (FindWall(mapX - 1, mapY + 1) != -1 && textureX > 0.5f) {
                    float ambMult = CalcAmbientMult(1.0f - textureX);
                    ambient = ambMult;
                }

                if (FindWall(mapX - 1, mapY - 1) != -1 && textureX < 0.5f) {
                    float ambMult = CalcAmbientMult(textureX);
                    ambient = ambMult;
                }
            }
            else {
                if (FindWall(mapX + 1, mapY + 1) != -1 && textureX > 0.5f) {
                    float ambMult = CalcAmbientMult(1.0f - textureX);
                    ambient = ambMult;
                }

                if (FindWall(mapX + 1, mapY - 1) != -1 && textureX < 0.5f) {
                    float ambMult = CalcAmbientMult(textureX);
                    ambient = ambMult;
                }
            }
        }
        else {
            if (rayDirY >= 0) {
                if (FindWall(mapX + 1, mapY - 1) != -1 && textureX > 0.5f) {
                    float ambMult = CalcAmbientMult(1.0f - textureX);
                    ambient = ambMult;
                }

                if (FindWall(mapX - 1, mapY - 1) != -1 && textureX < 0.5f) {
                    float ambMult = CalcAmbientMult(textureX);
                    ambient = ambMult;
                }
            }
            else {
                if (FindWall(mapX + 1, mapY + 1) != -1 && textureX > 0.5f) {
                    float ambMult = CalcAmbientMult(1.0f - textureX);
                    ambient = ambMult;
                }

                if (FindWall(mapX - 1, mapY + 1) != -1 && textureX < 0.5f) {
                    float ambMult = CalcAmbientMult(textureX);
                    ambient = ambMult;
                }
            }
        }

        //Fetch texture index
        int wallTextureIndex = worldWalls[WallIndex].textureIndex;

        return RayHit(
            perpWallDist,
            textureX,
            ambient,
            wallTextureIndex);
    }

    return RayHit(-1.0f, 0.0f, 0.0f, 0);
}