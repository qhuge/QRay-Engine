#pragma once

#include <windows.h>

#include "config.hpp"

extern int gSelectedTileType;
extern int gSelectedEntityType;
extern EditorCreateMode gCreateMode;

extern int gDoorDirection;

bool InitEditor(HINSTANCE hInstance, int nCmdShow);

int RunEditorLoop();

void ShutdownEditor();