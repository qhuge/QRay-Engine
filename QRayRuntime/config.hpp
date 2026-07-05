#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include "types.hpp"

struct GameConfig {
	char title[64];

	float playerX;

	float playerY;

	int renderDistance;

	int WINDOW_WIDTH;

	int WINDOW_HEIGHT;

	float angleOffset;

	float moveSpeed;

	float runSpeed;

	float angleSpeed;

	float lightDecay;

	float FOV;

	uint16_t textureAmount;

	int entityAmount;

	uint32_t ceilingColor;

	uint32_t floorColor;
};

extern GameConfig cfg;

bool InitConfig();

struct Texture
{
	uint32_t width;
	uint32_t height;
	std::vector<uint32_t> pixels;
};
struct QRayTextureAssetHeader
{
	uint32_t width;
	uint32_t height;
	uint32_t channels;
};

extern std::vector<Texture> gTextures;

extern std::vector<EntityType> gEntityTypes;

extern std::vector<int> gPlayerTags;