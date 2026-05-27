#pragma once
#include <vector>

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

	int textureAmount;
};

extern GameConfig cfg;

bool InitConfig();

struct Texture
{
	uint32_t width;
	uint32_t height;
	std::vector<unsigned char> pixels;
};
struct QRayTextureAssetHeader
{
	uint32_t width;
	uint32_t height;
	uint32_t channels;
};

extern std::vector<Texture> gTextures;