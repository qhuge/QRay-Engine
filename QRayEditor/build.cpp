#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>

#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "config.hpp"
#include "build.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::string GetExecutableDirectory()
{
	char path[MAX_PATH];

	GetModuleFileNameA(nullptr, path, MAX_PATH);

	std::string fullPath = path;

	size_t slash = fullPath.find_last_of("\\/");

	return fullPath.substr(0, slash);
}

void WriteMap(std::string buildFolder, std::vector<Tile> tiles, std::vector<Entity> ents, GameConfig& cfg) {
	std::ofstream outfile(buildFolder + "\\map.txt");

	for (int i = 0; i < tiles.size(); i++) {
		Tile& currentTile = tiles[i];

		//-1 means its the spawnpoint. dont write that to the actual map.
		if (currentTile.tileTypeIndex >= 0) {
			
			std::string newString;

			if (currentTile.doorDirection != DoorDirection::None) {
				//wtf is the line below
				newString = "D " + std::to_string(currentTile.x) + " " + std::to_string(currentTile.y) + " " + std::to_string(currentTile.tileTypeIndex) + " " + (currentTile.doorDirection == DoorDirection::Horizontal ? "0" : "1") + " " + (currentTile.doorDirection == DoorDirection::Horizontal ? "1" : "2") + " " + std::to_string(save.tileTypes[currentTile.tileTypeIndex].tag) + "\nD " + std::to_string(currentTile.x + (currentTile.doorDirection == DoorDirection::Horizontal ? 1 : 0)) + " " + std::to_string(currentTile.y + (currentTile.doorDirection == DoorDirection::Horizontal ? 0 : 1)) + " " + std::to_string(currentTile.tileTypeIndex) + " " + (currentTile.doorDirection == DoorDirection::Horizontal ? "0" : "1") + " " + (currentTile.doorDirection == DoorDirection::Horizontal ? "3" : "0") + " " + std::to_string(save.tileTypes[currentTile.tileTypeIndex].tag);
			}
			else {
				newString = "T " + std::to_string(currentTile.x) + " " + std::to_string(currentTile.y) + " " + std::to_string(currentTile.tileTypeIndex);
			}

			outfile << newString << std::endl;
		} else {
			//-1 is the spawnpoint tile
			//-2 is the other part of door.
			if (currentTile.tileTypeIndex == -1) {
				cfg.playerX = currentTile.x + 0.5f;
				cfg.playerY = currentTile.y + 0.5f;
			}
		}
	}

	for (int i = 0; i < ents.size(); i++) {
		Entity& currentEntity = ents[i];
		std::string newString = "E " + std::to_string(currentEntity.x) + " " + std::to_string(currentEntity.y) + " " + std::to_string(currentEntity.entityTypeIndex + (cfg.textureAmount - cfg.entityAmount));

		outfile << newString << std::endl;
	}

	outfile.close();
}

void WriteData(const GameConfig& config, std::string buildFolder)
{
	std::ofstream file(buildFolder + "\\data.qraydata", std::ios::binary);

	file.write((char*)&config, sizeof(GameConfig));

	file.close();
}

bool ConvertPngToQRayAsset(const std::string& inputPng, const std::string& outputFile) {
	int width, height, channels;

	unsigned char* data = stbi_load(inputPng.c_str(), &width, &height, &channels, 4);

	//TODO: check dimensions of texture
	if (!data) {
		return false;
	}

	QRayTextureAssetHeader header;
	header.width = width;
	header.height = height;
	header.channels = 4;

	std::ofstream file(outputFile, std::ios::binary);

	file.write((char*)&header, sizeof(header));
	file.write((char*)data, width * height * 4);

	file.close();

	stbi_image_free(data);

	return true;
};

void WriteEntityFile(const EntityType& ent, std::string path)
{
	EntityTypeRuntime entr;
	entr.health = ent.health;
	entr.idleMovement = ent.idleMovement;
	entr.losMovement = ent.losMovement;
	entr.tag = ent.tag;

	std::ofstream file(path, std::ios::binary);

	file.write((char*)&entr, sizeof(EntityTypeRuntime));

	file.close();
}

void build(std::string path) {
	OutputDebugStringA("BUILD STARTED\n");

	//make new cfg
	GameConfig cfg;

	//convert title to string
	std::string title = save.gameTitle;

	//spaces to underscores for file names:
	std::replace(title.begin(), title.end(), ' ', '_');

	//build directory
	std::string buildFolder = path + "\\" + title;

	//create a folder for the build and then create assets folder.
	CreateDirectoryA(buildFolder.c_str(), nullptr);
	CreateDirectoryA((buildFolder + "\\assets").c_str(), nullptr);

	//copy runtime and rename it
	std::string runtimePath = GetExecutableDirectory() + "\\QRayRuntime.exe";

	//TODO switch to using RUNTIME_PATH global later.
	CopyFileA(runtimePath.c_str(), (buildFolder + "\\" + title + ".exe").c_str(), TRUE);

	//converting and writing textures (make a new list of tiles that have correct texture indexes.)
	//we dont want to modify the actual world map, since that might get edited and built again
	int amountOfSkippedTiles = 0;
	std::vector<Tile> newMap = save.tiles;
	for (int i = 0; i < save.tileTypes.size(); i++) {
		TileType& currentTileType = save.tileTypes[i];
		if (currentTileType.timesUsed > 0) {
			ConvertPngToQRayAsset((saveLocation + "\\" + currentTileType.texturePath), (buildFolder + "\\assets\\" + std::to_string(i - amountOfSkippedTiles) + ".qrayasset"));
		}
		else {
			//if we skip a texture we must not then build that
			for (int f = 0; f < newMap.size(); f++) {
				Tile& currentTile = newMap[f];
				if (currentTile.tileTypeIndex >= i) {
					currentTile.tileTypeIndex--;
				}
			}

			amountOfSkippedTiles++;
		}
	}
	int amountOfTileAssets = save.tileTypes.size() - amountOfSkippedTiles;
	int amountOfSkippedEntities = 0;
	std::vector<Entity> newEnt = save.entities;
	for (int i = 0; i < save.entityTypes.size(); i++) {
		EntityType& currentEntityType = save.entityTypes[i];
		if (currentEntityType.timesUsed > 0) {
			ConvertPngToQRayAsset((saveLocation + "\\" + currentEntityType.texturePath), (buildFolder + "\\assets\\" + std::to_string(i - amountOfSkippedEntities + amountOfTileAssets) + ".qrayasset"));
			WriteEntityFile(currentEntityType, (buildFolder + "\\assets\\" + std::to_string(i - amountOfSkippedEntities + amountOfTileAssets) + ".qrayentity"));
		}
		else {
			for (int f = 0; f < newEnt.size(); f++) {
				Entity& currentEntity = newEnt[f];
				if (currentEntity.entityTypeIndex >= i) {
					currentEntity.entityTypeIndex--;
				}
			}

			amountOfSkippedEntities++;
		}
	}
	int amountOfEntityAssets = save.entityTypes.size() - amountOfSkippedEntities;

	cfg.textureAmount = amountOfTileAssets + amountOfEntityAssets;
	cfg.WINDOW_HEIGHT = save.resolutionY;
	cfg.WINDOW_WIDTH = save.resolutionX;
	strcpy_s(cfg.title, save.gameTitle);
	cfg.entityAmount = amountOfEntityAssets;
	cfg.renderDistance = save.renderDistance;
	cfg.angleOffset = save.startingAngle;

	//ceil and floor colors
	cfg.ceilingColor = (uint32_t(save.ceilingColor[0] * 255.0f) << 16) | (uint32_t(save.ceilingColor[1] * 255.0f) << 8) | (uint32_t(save.ceilingColor[2] * 255.0f));
	cfg.floorColor = (uint32_t(save.floorColor[0] * 255.0f) << 16) | (uint32_t(save.floorColor[1] * 255.0f) << 8) | (uint32_t(save.floorColor[2] * 255.0f));


	//write map file
	WriteMap(buildFolder, newMap, newEnt, cfg);

	//write data file
	WriteData(cfg, buildFolder);
}