#pragma once

struct GameConfig {
	char title[64];

	float playerX;

	float playerY;

	int renderDistance;

	int WINDOW_WIDTH;

	int WINDOW_HEIGHT;

	float scale;

	float angleOffset;

	float moveSpeed;

	float runSpeed;

	float angleSpeed;

	float lightDecay;

	float FOV;
};

extern GameConfig cfg;

bool InitConfig();