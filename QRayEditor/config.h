#pragma once
#include <string>

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

	float scale = 8.0f;

	float angleOffset = 90.0f;

	float moveSpeed = 0.01f;

	float runSpeed = 0.02f;

	float angleSpeed = 0.5f;

	float lightDecay = 0.02f;

	float FOV = 80.0f;
};

extern GameConfig cfg;