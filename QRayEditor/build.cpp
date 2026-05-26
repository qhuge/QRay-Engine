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

	GetModuleFileNameA(
		nullptr,
		path,
		MAX_PATH);

	std::string fullPath = path;

	size_t slash =
		fullPath.find_last_of("\\/");

	return fullPath.substr(0, slash);
}

void build(std::string path) {

	//build directory
	std::string buildFolder = path + "\\" + GAME_TITLE;

	//create a folder for the build
	CreateDirectoryA(buildFolder.c_str(),nullptr);

	//copy runtime and rename it
	std::string runtimePath = GetExecutableDirectory() + "\\QRayRuntime.exe";

	//TODO switch to using RUNTIME_PATH global later.
	CopyFileA(runtimePath.c_str(), (buildFolder + "\\" + GAME_TITLE + ".exe").c_str(), TRUE);

	//write map file
	std::ofstream outfile(buildFolder + "\\map.txt");
	
	for (int i = 0; i < worldMap.size(); i++) {
		Tile currentTile = worldMap[i];
		std::string newString = std::to_string(currentTile.x) + " " + std::to_string(currentTile.y) + " 0";

		outfile << newString << std::endl;
	}

	outfile.close();
}