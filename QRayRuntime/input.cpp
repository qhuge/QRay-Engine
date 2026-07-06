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
const float pickUpRadiusSquared = pickUpRadius * pickUpRadius;

//Process the entity pickups
void processEntities() {

    //loop over all entities
    for (auto& entity : worldEntities) {

        //if entity tag is 0, then it doesnt give any tag. Also skip hidden entities. TODO: add the heatlh check to only give tag if health is -1 (static entity)
        if (gEntityTypes[entity.entityTypeIndex].tag == 0 || !entity.active) {
            continue;
        }

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
}