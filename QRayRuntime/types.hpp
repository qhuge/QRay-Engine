#pragma once
#include <cstdint>

enum IdleMovementType
{
    IDLE_NONE = 0,
    IDLE_RANDOM_WALK = 1
};

enum LOSMovementType
{
    LOS_NONE = 0,
    LOS_SHOOT_PLAYER = 1,
    LOS_CHARGE_PLAYER = 2
};

struct EntityType
{
    IdleMovementType idleMovement;
    LOSMovementType losMovement;
    int health;
    uint32_t tag;
};