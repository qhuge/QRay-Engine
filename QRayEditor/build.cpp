#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>

#include <string>
#include <iostream>
#include <fstream>

#include "config.h"
#include "build.h"
#include "editor_map.h";

std::string GetExecutableDirectory()
{
	char path[MAX_PATH];

	GetModuleFileNameA(nullptr, path, MAX_PATH);

	std::string fullPath = path;

	size_t slash = fullPath.find_last_of("\\/");

	return fullPath.substr(0, slash);
}

void WriteMap(std::string buildFolder) {
	std::ofstream outfile(buildFolder + "\\map.txt");

	for (int i = 0; i < worldMap.size(); i++) {
		Tile currentTile = worldMap[i];
		std::string newString = std::to_string(currentTile.x) + " " + std::to_string(currentTile.y) + " 0";

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

void build(std::string path) {
	OutputDebugStringA(path.c_str());
	OutputDebugStringA("\n");

	OutputDebugStringA("BUILD STARTED\n");
	//convert title to string
	std::string title = cfg.title;

	OutputDebugStringA(title.c_str());
	OutputDebugStringA("\n");

	//build directory
	std::string buildFolder = path + "\\" + title;

	//create a folder for the build
	CreateDirectoryA(buildFolder.c_str(),nullptr);

	OutputDebugStringA("FOLDER CREATED\n");

	//copy runtime and rename it
	std::string runtimePath = GetExecutableDirectory() + "\\QRayRuntime.exe";

	//TODO switch to using RUNTIME_PATH global later.
	CopyFileA(runtimePath.c_str(), (buildFolder + "\\" + title + ".exe").c_str(), TRUE);

	OutputDebugStringA("RUNTIME COPIED\n");

	//write map file
	WriteMap(buildFolder);

	OutputDebugStringA("MAP FILE WRITTEN\n");

	//write data file
	WriteData(cfg, buildFolder);

	OutputDebugStringA("DATA FILE WRITTEN\n");

	OutputDebugStringA("BUILD COMPLETED\n");
}