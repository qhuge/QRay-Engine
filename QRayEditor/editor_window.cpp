#define NOMINMAX

#include "editor_window.h"
#include "editor_renderer.h"
#include "editor_map.h"
#include <string>
#include "editor_helpers.h"
#include "build.h"
#include "editor_settings_ui.h"
#include "config.h"
#include "editor_viewport.h"
#include "editor_add_block_ui.h"

#define ID_FILE_SAVE 1001
#define ID_FILE_LOAD 1002
#define ID_FILE_BUILD 1004
#define ID_FILE_EXIT 1003
#define ID_FILE_SETTINGS 1005
#define ID_ADD_BLOCK 5001

static EditorState* gEditor = nullptr;

static HWND gViewportWindow = nullptr;
static HWND gAddBlockButton = nullptr;
static HWND gBlockList = nullptr;
static HWND gNewMapButton = nullptr;
static HWND gSaveButton = nullptr;
static HWND gBlockLabel = nullptr;

void RefreshBlockList()
{
    SendMessageW(gBlockList, LB_RESETCONTENT, 0, 0);

    for (const BlockType& block : gBlockTypes)
    {
        std::wstring wName( block.name.begin(), block.name.end());

        SendMessageW(gBlockList, LB_ADDSTRING, 0, (LPARAM)wName.c_str());
    }

    SendMessageW(
        gBlockList,
        LB_ADDSTRING,
        0,
        (LPARAM)L"Spawnpoint");

    SendMessageW(gBlockList, LB_SETCURSEL, 0, 0);
}

bool InitEditorWindow(EditorState& editor, HINSTANCE hInstance, int nCmdShow)
{
    gEditor = &editor;

    WNDCLASSW wc = {};

    wc.lpfnWndProc = EditorWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"QRayEditorClass";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);

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

    WNDCLASSW viewportClass = {};
    viewportClass.style =CS_HREDRAW | CS_VREDRAW;
    viewportClass.lpfnWndProc = ViewportWndProc;
    viewportClass.hInstance = hInstance;
    viewportClass.lpszClassName = L"QRayViewportClass";
    viewportClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    viewportClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassW(&viewportClass);

    gViewportWindow = CreateWindowW(
        L"QRayViewportClass",
        L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        0,
        0,
        800,
        600,
        hWnd,
        nullptr,
        hInstance,
        nullptr);

    if (!gViewportWindow)
    {
        MessageBoxA(
            nullptr,
            "Viewport creation failed",
            "Error",
            MB_OK);

        return false;
    }

    gAddBlockButton = CreateWindowW(
        L"BUTTON",
        L"+",
        WS_CHILD | WS_VISIBLE,
        820,
        20,
        40,
        40,
        hWnd,
        (HMENU)ID_ADD_BLOCK,
        hInstance,
        nullptr);

    gBlockLabel = CreateWindowW(
        L"STATIC",
        L"Block Types",
        WS_CHILD | WS_VISIBLE,
        870,
        30,
        120,
        20,
        hWnd,
        nullptr,
        hInstance,
        nullptr);

    gBlockList = CreateWindowW(
        L"LISTBOX",
        nullptr,
        WS_CHILD |
        WS_VISIBLE |
        WS_BORDER |
        LBS_NOTIFY,
        820,
        70,
        200,
        200,
        hWnd,
        (HMENU)3001,
        hInstance,
        nullptr);

    RefreshBlockList();

    /*gNewMapButton = CreateWindowW(
        L"BUTTON",
        L"New Map",
        WS_CHILD | WS_VISIBLE,
        820,
        250,
        200,
        40,
        hWnd,
        (HMENU)4001,
        hInstance,
        nullptr);*/

    /*gSaveButton = CreateWindowW(
        L"BUTTON",
        L"Save Map",
        WS_CHILD | WS_VISIBLE,
        820,
        300,
        200,
        40,
        hWnd,
        (HMENU)4002,
        hInstance,
        nullptr);*/

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

    RECT rect;

    GetClientRect(hWnd, &rect);

    SendMessage(hWnd, WM_SIZE, 0, MAKELPARAM(rect.right, rect.bottom));

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return true;
}

LRESULT CALLBACK EditorWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
            case ID_FILE_SAVE:
                SaveMap();
                break;

            case ID_FILE_LOAD:
                LoadMap();
                InvalidateRect(gViewportWindow, nullptr, TRUE);
                break;

            case ID_FILE_EXIT:
                PostQuitMessage(0);
                break;
            case ID_FILE_BUILD:
            {
                if (!hasSelectedSpawnPoint) {
                    MessageBoxW(hWnd, L"No spawnpoint chosen. Choose a spawnpoint before building the game!", L"OK", MB_OK);
                    break;
                }
                std::string buildLocation = SelectFolder();
                if (buildLocation.length() == 0) {
                    MessageBoxW(hWnd, L"Invalid build path", L"OK", MB_OK);
                    break;
                }
                build(buildLocation);
                break;
            }
            case ID_FILE_SETTINGS:
                OpenSettingsWindow(gEditor->hInst, hWnd, cfg);
                break;

            case 3001:
            {
                if (HIWORD(wParam) == LBN_SELCHANGE)
                {
                    gSelectedBlockType = (int)SendMessageW(gBlockList, LB_GETCURSEL, 0, 0);
                }
            }
            break;

            case ID_ADD_BLOCK:
            {
                OpenAddBlockWindow(gEditor->hInst, hWnd);
            }
            break;
            }
            
        }
        break;

        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }

        case WM_SIZE:
        {
            RECT clientRect;

            GetClientRect(hWnd, &clientRect);

            int width = clientRect.right;
            int height = clientRect.bottom;

            int rightPanelWidth = 240;

            MoveWindow(
                gAddBlockButton,
                width - rightPanelWidth + 20,
                20,
                40,
                40,
                TRUE);

            MoveWindow(
                gBlockLabel,
                width - rightPanelWidth + 70,
                30,
                120,
                20,
                TRUE);

            MoveWindow(
                gViewportWindow,
                0,
                0,
                width - rightPanelWidth,
                height,
                TRUE);

            MoveWindow(
                gBlockList,
                width - rightPanelWidth + 20,
                70,
                200,
                200,
                TRUE);

            MoveWindow(
                gNewMapButton,
                width - rightPanelWidth + 20,
                250,
                200,
                40,
                TRUE);

            MoveWindow(
                gSaveButton,
                width - rightPanelWidth + 20,
                300,
                200,
                40,
                TRUE);

            RedrawWindow(
                hWnd,
                nullptr,
                nullptr,
                RDW_INVALIDATE |
                RDW_ERASE |
                RDW_ALLCHILDREN);

            return 0;
        }
    return 0;
    }

    return DefWindowProc(
        hWnd,
        message,
        wParam,
        lParam);
}