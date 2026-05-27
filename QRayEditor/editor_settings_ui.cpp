#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>

#include "editor_settings_ui.h"
#include "config.h"

static GameConfig* gConfig = nullptr;

// control IDs
#define ID_TITLE_EDIT 1001
#define ID_WIDTH_EDIT 1002
#define ID_HEIGHT_EDIT 1003
#define ID_SAVE_BTN 2001

static HWND hTitleEdit;
static HWND hWidthEdit;
static HWND hHeightEdit;

LRESULT CALLBACK SettingsWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        CreateWindowW(L"STATIC", L"Title:",
            WS_CHILD | WS_VISIBLE,
            20, 20, 80, 20,
            hWnd, nullptr, nullptr, nullptr);

        hTitleEdit = CreateWindowW(L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            120, 20, 200, 20,
            hWnd, (HMENU)ID_TITLE_EDIT, nullptr, nullptr);

        CreateWindowW(L"STATIC", L"Width:",
            WS_CHILD | WS_VISIBLE,
            20, 60, 80, 20,
            hWnd, nullptr, nullptr, nullptr);

        hWidthEdit = CreateWindowW(L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            120, 60, 200, 20,
            hWnd, (HMENU)ID_WIDTH_EDIT, nullptr, nullptr);

        CreateWindowW(L"STATIC", L"Height:",
            WS_CHILD | WS_VISIBLE,
            20, 100, 80, 20,
            hWnd, nullptr, nullptr, nullptr);

        hHeightEdit = CreateWindowW(L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            120, 100, 200, 20,
            hWnd, (HMENU)ID_HEIGHT_EDIT, nullptr, nullptr);

        CreateWindowW(L"BUTTON", L"Save",
            WS_CHILD | WS_VISIBLE,
            120, 150, 100, 30,
            hWnd, (HMENU)ID_SAVE_BTN, nullptr, nullptr);

        SetWindowTextA(hTitleEdit, gConfig->title);
        wchar_t sizeBuffer[32];

        wsprintfW(sizeBuffer, L"%d", gConfig->WINDOW_WIDTH);
        SetWindowTextW(hWidthEdit, sizeBuffer);

        wsprintfW(sizeBuffer, L"%d", gConfig->WINDOW_HEIGHT);
        SetWindowTextW(hHeightEdit, sizeBuffer);
    }
    break;

    case WM_COMMAND:
    {
        if (LOWORD(wParam) == ID_SAVE_BTN)
        {
            wchar_t buffer[64];

            // TITLE
            GetWindowTextW(hTitleEdit, buffer, 64);
            WideCharToMultiByte(CP_UTF8, 0,
                buffer, -1,
                gConfig->title, 64,
                nullptr, nullptr);

            // WIDTH
            GetWindowTextW(hWidthEdit, buffer, 64);
            gConfig->WINDOW_WIDTH = _wtoi(buffer);

            // HEIGHT
            GetWindowTextW(hHeightEdit, buffer, 64);
            gConfig->WINDOW_HEIGHT = _wtoi(buffer);

            DestroyWindow(hWnd);
        }
    }
    break;

    case WM_DESTROY:
        return 0;
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

bool OpenSettingsWindow(HINSTANCE hInstance, HWND parent, GameConfig& config)
{
    gConfig = &config;

    WNDCLASSW wc = {};
    wc.lpfnWndProc = SettingsWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"QRaySettingsClass";

    static bool registered = false;

    if (!registered)
    {
        RegisterClassW(&wc);
        registered = true;
    }

    HWND hWnd = CreateWindowW(
        wc.lpszClassName,
        L"Settings",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        400,
        250,
        nullptr,
        nullptr,
        hInstance,
        nullptr);

    if (!hWnd)
        return false;

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    return true;
}