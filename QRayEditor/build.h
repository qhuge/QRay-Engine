#pragma once
#include <string>
#include "editor_map.h"

std::string GetExecutableDirectory();

void WriteMap(std::string buildFolder, std::vector<Tile> mapToWrite);

void WriteData(const GameConfig& config, std::string buildFolder);

bool ConvertPngToQRayAsset(const std::string& inputPng, const std::string& outputFile);

void build(std::string path);