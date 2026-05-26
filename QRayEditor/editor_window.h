#pragma once

#define NOMINMAX

#include <windows.h>

struct EditorState
{
    HINSTANCE hInst;
    HWND hWnd;
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