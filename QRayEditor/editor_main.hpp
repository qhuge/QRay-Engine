#pragma once

#include <windows.h>

#include "config.hpp"

extern int gSelectedTileType;
extern int gSelectedEntityType;

extern EditorCreateMode gCreateMode;

bool InitEditor(HINSTANCE hInstance, int nCmdShow);

int RunEditorLoop();

void ShutdownEditor();