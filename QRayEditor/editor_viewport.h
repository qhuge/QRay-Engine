#pragma once

#include <windows.h>

extern int gCameraX;
extern int gCameraY;

extern bool gDragging;

extern int gLastMouseX;
extern int gLastMouseY;

extern int gSelectedBlockType;

extern bool hasSelectedSpawnPoint;

LRESULT CALLBACK ViewportWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);