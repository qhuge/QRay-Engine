#define _USE_MATH_DEFINES
#define NOMINMAX

#include <cmath>
#include "config.hpp"
#include "window_win32.hpp"
#include "helperFunctions.hpp"
#include "input.hpp"

//Process the input with the keyboard, and movement
void processInput() {

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

    if (moveX != 0.0f && CanMoveTo(newX, cfg.playerY))
    {
        cfg.playerX = newX;
    }

    if (moveY != 0.0f && CanMoveTo(cfg.playerX, newY))
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
}

const float pickUpRadius = 0.5f;
const float movementRadius = 0.2f;
const float movementSpeed = 0.5f;

const float pickUpRadiusSquared = pickUpRadius * pickUpRadius;
const float movementRadiusSquared = movementRadius * movementRadius;
const int offsets[4][2] =
{
    { 0, -1},
    {-1,  0},
    { 1,  0},
    { 0,  1},
};
//Process the entity pickups and movement
void processEntities() {

    //loop over all entities
    for (auto& entity : worldEntities) {

        //skip unactive entities
        if (!entity.active) {
            continue;
        }

        //if entity tag isnt 0 and it has -1 health. TODO: handle tag awarding from enemies when damaging them
        if (gEntityTypes[entity.entityTypeIndex].tag != 0 && entity.health == -1) {

            //calulate the difference in each coordinate
            float dx = cfg.playerX - entity.x;
            float dy = cfg.playerY - entity.y;

            //compare distances squared
            float distanceSquared = (dx * dx) + (dy * dy);

            if (distanceSquared <= pickUpRadiusSquared) {
                entity.active = false;
                gPlayerTags.insert(gEntityTypes[entity.entityTypeIndex].tag);
            }
        }

        //handle movement: (static entities dont move)
        if (gEntityTypes[entity.entityTypeIndex].idleMovement == IDLE_RANDOM_WALK && entity.health != -1) {
            //Check if we are close enough to the location:
            float dx = entity.targetX - entity.x;
            float dy = entity.targetY - entity.y;

            float distanceSquared = (dx * dx) + (dy * dy);

            if (distanceSquared <= movementRadiusSquared) {
                //calculate new target location:
                std::vector<std::pair<int, int>> candidates;
                int tileX = (int)floorf(entity.x);
                int tileY = (int)floorf(entity.y);
                for (int i = 0; i < 4; i++)
                {
                    int x = tileX + offsets[i][0];
                    int y = tileY + offsets[i][1];

                    if (!isWallAt(x, y))
                    {
                        candidates.emplace_back(x, y);
                    }
                }

                if (!candidates.empty())
                {
                    int index = rand() % candidates.size();
                    entity.targetX = candidates[index].first + 0.5f;
                    entity.targetY = candidates[index].second + 0.5f;

                    //recalculate the vector and distance
                    dx = entity.targetX - entity.x;
                    dy = entity.targetY - entity.y;
                    distanceSquared = (dx * dx) + (dy * dy);
                }
            }

            //this is some woodoo shit google gave me:
            float xhalf = 0.5f * distanceSquared;
            int i = *(int*)&distanceSquared;
            i = 0x5f3759df - (i >> 1);
            float invDistance = *(float*)&i;
            invDistance = invDistance * (1.5f - xhalf * invDistance * invDistance);

            //unit vector?
            float ux = dx * invDistance;
            float uy = dy * invDistance;

            //move along said unit vecotr
            entity.x += ux * movementSpeed * deltaTime;
            entity.y += uy * movementSpeed * deltaTime;
        }
    }
}