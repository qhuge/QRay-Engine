#pragma once

#include <vector>

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

//the 2 door TILES are placed in horizontal order.
enum DoorDirection
{
    None = 0,
    Horizontal = 1,
    Vertical = 2,
};

struct Tile
{
	int x;
	int y;
    int tileTypeIndex;

    DoorDirection doorDirection = DoorDirection::None; //the 2 door TILES are placed in horizontal order. (note to self later: what the fuck does this comment even mean??)
};
struct TileType
{
	char name[64];

	char texturePath[260];

    int frameWidth, frameHeight;

	int r, g, b;

	int timesUsed = 0;

    bool isDoor = false;

    int tag = 0; //this only applies when isDoor = true!!!!!!!!!!!
};

struct Entity
{
	int x;
	int y;
    uint16_t entityTypeIndex;
};

struct EntityType
{
    char name[64];
    char texturePath[260];

    int frameHeight, frameWidth;
    float frameTime;

    int r, g, b;

    int timesUsed = 0;

    IdleMovementType idleMovement;
    LOSMovementType losMovement;
    int health; // -1 = takes no damage
    uint32_t tag; //0 = no tag given, if health == -1 then tag is given when player gets close, otherwise given when health == 0
    bool active; //is entity rendered?
    //TODO: the active bool does nothing as its not in the build files
};

struct EntityTypeRuntime
{
    IdleMovementType idleMovement;
    LOSMovementType losMovement;
    int health; // -1 = takes no damage
    uint32_t tag; //0 = no tag given, if health == -1 then tag is given when player gets close, otherwise given when health == 0
};