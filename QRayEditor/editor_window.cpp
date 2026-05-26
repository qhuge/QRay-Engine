#define NOMINMAX

#include "editor_window.h"
#include "editor_renderer.h"
#include "editor_map.h"
#include "config.h"
#include <string>
#include "editor_helpers.h"
#include "build.h"
#include "editor_settings_ui.h"
#include "config.h"

#define ID_FILE_SAVE 1001
#define ID_FILE_LOAD 1002
#define ID_FILE_BUILD 1004
#define ID_FILE_EXIT 1003
#define ID_FILE_SETTINGS 1005

static EditorState* gEditor = nullptr;

bool InitEditorWindow(
    EditorState& editor,
    HINSTANCE hInstance,
    int nCmdShow)
{
    gEditor = &editor;

    WNDCLASSW wc = {};

    wc.lpfnWndProc = EditorWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"QRayEditorClass";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClassW(&wc);

    HWND hWnd = CreateWindowW(
        L"QRayEditorClass",
        L"QRay Editor",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        nullptr,
        nullptr,
        hInstance,
        nullptr);

    if (!hWnd)
    {
        return false;
    }

    editor.hWnd = hWnd;
    editor.hInst = hInstance;

    HMENU hMenuBar = CreateMenu();
    HMENU hFileMenu = CreatePopupMenu();

    /*AppendMenuW(
        hFileMenu,
        MF_STRING,
        ID_FILE_SAVE,
        L"Save");

    AppendMenuW(
        hFileMenu,
        MF_STRING,
        ID_FILE_LOAD,
        L"Load");*/

    AppendMenuW(
        hFileMenu,
        MF_STRING,
        ID_FILE_BUILD,
        L"Build");
    AppendMenuW(
        hFileMenu,
        MF_STRING,
        ID_FILE_SETTINGS,
        L"Settings");

    AppendMenuW(
        hFileMenu,
        MF_SEPARATOR,
        0,
        nullptr);

    AppendMenuW(
        hFileMenu,
        MF_STRING,
        ID_FILE_EXIT,
        L"Exit");

    AppendMenuW(
        hMenuBar,
        MF_POPUP,
        (UINT_PTR)hFileMenu,
        L"File");

    SetMenu(hWnd, hMenuBar);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return true;
}

LRESULT CALLBACK EditorWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_LBUTTONDOWN:
        {
            int mouseX = LOWORD(lParam);
            int mouseY = HIWORD(lParam);

            int tileX = mouseX / TILE_SIZE;
            int tileY = mouseY / TILE_SIZE;

            if (tileX >= 0 &&
                tileX < MAP_WIDTH &&
                tileY >= 0 &&
                tileY < MAP_HEIGHT)
            {
                worldMap.push_back(Tile { tileX, tileY, 0});
                //worldMap[tileY][tileX] = 1;
            }

            InvalidateRect(hWnd, nullptr, FALSE);
        }
        break;

        case WM_RBUTTONDOWN:
        {
            int mouseX = LOWORD(lParam);
            int mouseY = HIWORD(lParam);

            int tileX = mouseX / TILE_SIZE;
            int tileY = mouseY / TILE_SIZE;

            if (tileX >= 0 &&
                tileX < MAP_WIDTH &&
                tileY >= 0 &&
                tileY < MAP_HEIGHT)
            {
                worldMap.erase(worldMap.begin() + FindWall(tileX, tileY));
                //worldMap[tileY][tileX] = 0;
            }

            InvalidateRect(hWnd, nullptr, FALSE);
        }
        break;

        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
            case ID_FILE_SAVE:
                SaveMap();
                break;

            case ID_FILE_LOAD:
                LoadMap();
                InvalidateRect(hWnd, nullptr, FALSE);
                break;

            case ID_FILE_EXIT:
                PostQuitMessage(0);
                break;
            case ID_FILE_BUILD:
            {
                std::string buildLocation = SelectFolder();
                build(buildLocation);
                break;
            }
            case ID_FILE_SETTINGS:
                OpenSettingsWindow(gEditor->hInst, hWnd, cfg);
                break;
            }
        }
        break;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;

            HDC hdc = BeginPaint(hWnd, &ps);

            RenderEditor(hdc);

            EndPaint(hWnd, &ps);
        }
        return 0;

        case WM_DESTROY:
        {
            PostQuitMessage(0);
        }
    return 0;
    }

    return DefWindowProc(
        hWnd,
        message,
        wParam,
        lParam);
}