#pragma once
#include <string>
#include "world.hpp"

std::string GetExecutableDirectory();

void WriteMap(std::string buildFolder, std::vector<Tile> tiles, std::vector<Entity> ents, GameConfig& cfg);

void WriteData(const GameConfig& config, std::string buildFolder);

bool ConvertPngToQRayAsset(const std::string& inputPng, const std::string& outputFile, int frameWidth, int frameHeight);

void build(std::string path);

struct QRayTextureAssetHeader
{
	uint32_t width;
	uint32_t height;
	uint32_t frames; //for animation
	float maxAnimationDuration;//for anim
	uint32_t channels;
};