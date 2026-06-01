#pragma once
#include <vector>
#include "world.hpp"
#include <string>

struct GameConfig {
	char title[64]{ "Example Game" };

	float playerX;

	float playerY;

	int renderDistance;

	int WINDOW_WIDTH;

	int WINDOW_HEIGHT;

	float angleOffset;

	float moveSpeed = 3.0f;

	float runSpeed = 6.0f;

	float angleSpeed = 180.0f;

	float lightDecay = 0.1f;

	float FOV = 80.0f;

	uint16_t textureAmount;

	int entityAmount;
};

struct ProjectFile
{
	char gameTitle[64]{ "Example Game" };
	char projectTitle[64]{ "Example Project" };

	int resolutionX = 640;
	int resolutionY = 400;
	int startingAngle = -90;
	int renderDistance = 30;

	bool hasPlacedSpawnpoint = false;

	std::vector<Tile> tiles;
	std::vector<Entity> entities;

	std::vector<TileType> tileTypes;
	std::vector<EntityType> entityTypes;
};

inline ProjectFile save;

inline std::string saveLocation;

enum class EditorState
{
	Startup,
	Editing
};

inline EditorState gEditorState = EditorState::Startup;

enum class EditorCreateMode {
	Tile,
	Entity,
	Spawnpoint
};