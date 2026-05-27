#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>

#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "config.h"
#include "build.h"
#include "editor_map.h";

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

void WriteMap(std::string buildFolder, std::vector<Tile> mapToWrite) {
	std::ofstream outfile(buildFolder + "\\map.txt");

	for (int i = 0; i < mapToWrite.size(); i++) {
		Tile currentTile = mapToWrite[i];
		
		//-1 means its the spawnpoint. dont write that to the actual map.
		if (currentTile.textureIndex != -1) {
			std::string newString = std::to_string(currentTile.x) + " " + std::to_string(currentTile.y) + " " + std::to_string(currentTile.textureIndex);

			outfile << newString << std::endl;
		}
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

	if (!data || (width != 32 || height != 32)) {
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

void build(std::string path) {
	OutputDebugStringA("BUILD STARTED\n");

	//convert title to string
	std::string title = cfg.title;

	//spaces to underscores for file names:
	std::replace(title.begin(), title.end(), ' ', '_');

	//build directory
	std::string buildFolder = path + "\\" + title;

	//create a folder for the build and then create assets folder.
	CreateDirectoryA(buildFolder.c_str(),nullptr);
	CreateDirectoryA((buildFolder + "\\assets").c_str(), nullptr);

	//copy runtime and rename it
	std::string runtimePath = GetExecutableDirectory() + "\\QRayRuntime.exe";

	//TODO switch to using RUNTIME_PATH global later.
	CopyFileA(runtimePath.c_str(), (buildFolder + "\\" + title + ".exe").c_str(), TRUE);

	//converting and writing textures (make a new list of tiles that have correct texture indexes.)
	//we dont want to modify the actual world map, since that might get edited and built again
	int amountOfSkippedTextures = 0;
	std::vector<Tile> newMap = worldMap;
	for (int i = 0; i < gBlockTypes.size(); i++) {
		BlockType currentBlockType = gBlockTypes[i];
		if (currentBlockType.timesUsed > 0) {
			ConvertPngToQRayAsset(currentBlockType.texturePath, (buildFolder + "\\assets\\" + std::to_string(i - amountOfSkippedTextures) + ".qrayasset"));
		}
		else {
			//if we skip a texture we must 
			for (int f = 0; f < newMap.size(); f++) {
				Tile& currentTile = newMap[f];
				if (currentTile.textureIndex >= i) {
					currentTile.textureIndex--;
				}
			}

			amountOfSkippedTextures++;
		}
	}

	
	cfg.textureAmount = gBlockTypes.size() - amountOfSkippedTextures;

	//write map file
	WriteMap(buildFolder, newMap);

	//write data file
	WriteData(cfg, buildFolder);
}