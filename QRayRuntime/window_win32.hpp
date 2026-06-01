#pragma once

#include <windows.h>
#include <cstdint>
#include <chrono>

#include "renderer.hpp"

//std::chrono::high_resolution_clock::time_point lastTime;
extern float deltaTime;

struct Win32State
{
    HINSTANCE hInst;
    HWND hWnd;

    Framebuffer framebuffer;

    BITMAPINFO bitmapInfo;

    WCHAR szTitle[256];
    WCHAR szWindowClass[256];
};

void ClearScreen(Framebuffer framebuffer, uint32_t color);

void PutPixel(Framebuffer framebuffer, int x, int y, uint32_t color);

bool KeyDown(int key);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow);

ATOM MyRegisterClass(Win32State& win32, HINSTANCE hInstance);

BOOL InitInstance(Win32State& win32, HINSTANCE hInstance, int nCmdShow);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);