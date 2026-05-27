#pragma once

#include <string>
#include <vector>

static const int WINDOW_WIDTH = 1280;
static const int WINDOW_HEIGHT = 720;

static const int TILE_SIZE = 32;

static const int MAP_WIDTH = 50;
static const int MAP_HEIGHT = 50;

static const std::string RUNTIME_PATH = "QRayRuntime.exe";

struct GameConfig {
	char title[64] = "Example Game";

	float playerX = 0.0f;

	float playerY = 0.0f;

	int renderDistance = 30;

	int WINDOW_WIDTH = 640;

	int WINDOW_HEIGHT = 400;

	float angleOffset = 90.0f;

	float moveSpeed = 3.0f;

	float runSpeed = 6.0f;

	float angleSpeed = 180.0f;

	float lightDecay = 0.1f;

	float FOV = 80.0f;

	int textureAmount = 0;
};

struct QRayTextureAssetHeader
{
	uint32_t width;
	uint32_t height;
	uint32_t channels;
};

extern GameConfig cfg;

struct BlockType
{
	std::string name;

	std::string texturePath;

	int timesUsed = 0;
};

extern std::vector<BlockType> gBlockTypes;