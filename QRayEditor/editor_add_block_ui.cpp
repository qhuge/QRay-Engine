#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <commdlg.h>

#include <string>

#include "editor_add_block_ui.hpp"
#include "editor_map.hpp"
#include "editor_window.hpp"

#include "editor_helpers.hpp"
#include <stb_image.h>

#define ID_NAME_EDIT     1001
#define ID_TEXTURE_EDIT  1002
#define ID_BROWSE_BTN    1003
#define ID_CREATE_BTN    1004

static HWND gNameEdit = nullptr;
static HWND gTextureEdit = nullptr;

LRESULT CALLBACK AddBlockWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            CreateWindowW(
                L"STATIC",
                L"Block Name:",
                WS_CHILD | WS_VISIBLE,
                20,
                20,
                100,
                20,
                hWnd,
                nullptr,
                nullptr,
                nullptr);
            gNameEdit = CreateWindowW(
                L"EDIT",
                L"",
                WS_CHILD | WS_VISIBLE | WS_BORDER,
                120,
                20,
                460,
                24,
                hWnd,
                (HMENU)ID_NAME_EDIT,
                nullptr,
                nullptr);
            CreateWindowW(
                L"STATIC",
                L"Texture:",
                WS_CHILD | WS_VISIBLE,
                20,
                60,
                100,
                20,
                hWnd,
                nullptr,
                nullptr,
                nullptr);
            gTextureEdit = CreateWindowW(
                L"EDIT",
                L"",
                WS_CHILD |
                WS_VISIBLE |
                WS_BORDER |
                ES_AUTOHSCROLL,
                120,
                60,
                460,
                24,
                hWnd,
                (HMENU)ID_TEXTURE_EDIT,
                nullptr,
                nullptr);
            CreateWindowW(
                L"BUTTON",
                L"Browse",
                WS_CHILD | WS_VISIBLE,
                590,
                60,
                80,
                24,
                hWnd,
                (HMENU)ID_BROWSE_BTN,
                nullptr,
                nullptr);
            CreateWindowW(
                L"BUTTON",
                L"Create",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                120,
                120,
                100,
                30,
                hWnd,
                (HMENU)ID_CREATE_BTN,
                nullptr,
                nullptr);
            break;
        }

        case WM_COMMAND:
        {
            if (LOWORD(wParam) == ID_BROWSE_BTN)
            {
                OPENFILENAMEW ofn = {};

                wchar_t fileName[MAX_PATH] = L"";

                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hWnd;
                ofn.lpstrFile = fileName;
                ofn.nMaxFile = MAX_PATH;

                ofn.lpstrFilter = L"PNG Files\0*.png\0All Files\0*.*\0";

                if (GetOpenFileNameW(&ofn))
                {
                    SetWindowTextW(gTextureEdit, fileName);
                    SetForegroundWindow(hWnd);
                }
            }
            if (LOWORD(wParam) == ID_CREATE_BTN)
            {
                wchar_t nameBuffer[128];
                wchar_t textureBuffer[MAX_PATH];

                GetWindowTextW(gNameEdit, nameBuffer, 128);

                GetWindowTextW(gTextureEdit, textureBuffer, MAX_PATH);

                std::wstring wName = nameBuffer;
                std::wstring wTexture = textureBuffer;

                std::string name(wName.begin(), wName.end());

                std::string texturePath(wTexture.begin(), wTexture.end());

                if (!name.empty() && !texturePath.empty()) {
                    BlockType block;

                    block.name = name;
                    block.texturePath = texturePath;

                    block.colorR = (rand() % 236) + 20;
                    block.colorG = (rand() % 236) + 20;
                    block.colorB = (rand() % 236) + 20;

                    gBlockTypes.push_back(block);

                    RefreshBlockList();
                }
             
                DestroyWindow(hWnd);
            }
            break;
        }

        case WM_DESTROY:
            return 0;

        case WM_CLOSE:
        {
            DestroyWindow(hWnd);
            return 0;
        }
    }

    return DefWindowProcW(
        hWnd,
        msg,
        wParam,
        lParam);
}

bool OpenAddBlockWindow(HINSTANCE hInstance,HWND parent)
{
    WNDCLASSW wc = {};

    wc.lpfnWndProc = AddBlockWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"QRayAddBlockClass";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClassW(&wc);

    HWND hWnd = CreateWindowW(
        wc.lpszClassName,
        L"Add Block",
        WS_OVERLAPPED |
        WS_CAPTION |
        WS_SYSMENU,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        700,
        220,
        nullptr,
        nullptr,
        hInstance,
        nullptr);

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    return true;
}