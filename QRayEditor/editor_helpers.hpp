#pragma once
#include "windows.h"

#include "config.hpp"
#include "world.hpp"
#include <string>

int FindWall(int x, int y);

int FindEntity(int x, int y);

void SaveProject(const std::string& path);

void LoadProject(const std::string& path, HWND g_hWnd);

std::string SelectFolder();

std::string SelectProjectFile();

void CreateNewProject(const std::string& folder, const std::string projectName);

std::string OpenFileDialogPNG();

bool CopyAssetToProject(const std::string& sourcePath, const std::string& projectFolder, std::string& outRelativePath);

std::string GenerateAssetName(const std::string& original);

void SetRandomColorForTile(TileType& t);

void SetRandomColorForEntity(EntityType& e);

void SetEditorMode(EditorCreateMode mode);

bool fileExists(const std::string& filename);

bool FloatEquals(float a, float b);