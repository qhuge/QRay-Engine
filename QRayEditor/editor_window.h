#pragma once

#define NOMINMAX

#include <windows.h>

#include "config.h"

void RefreshBlockList();

struct EditorState
{
    HINSTANCE hInst;
    HWND hWnd;
    GameConfig config;
};

bool InitEditorWindow(
    EditorState& editor,
    HINSTANCE hInstance,
    int nCmdShow);

LRESULT CALLBACK EditorWndProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam);