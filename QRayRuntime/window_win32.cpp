#define NOMINMAX

#include "window_win32.h"
#include "config.h"
#include <cstdint>
#include "renderer.h"
#include "world.h"
#include <string>
#include <chrono>

static auto lastTime = std::chrono::high_resolution_clock::now();

float deltaTime = 0.0f;

static Win32State* gWin32 = nullptr;

void ClearScreen(Win32State& win32, uint32_t color)
{
    for (int i = 0; i < cfg.WINDOW_WIDTH * cfg.WINDOW_HEIGHT; i++)
    {
        win32.pixels[i] = color;
    }
}

void PutPixel(Win32State& win32, int x, int y, uint32_t color)
{
    if (x < 0 || x >= cfg.WINDOW_WIDTH ||
        y < 0 || y >= cfg.WINDOW_HEIGHT)
    {
        return;
    }

    win32.pixels[y * cfg.WINDOW_WIDTH + x] = color;
}

bool KeyDown(int key)
{
    return (GetAsyncKeyState(key) & 0x8000);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    bool initSuccess = InitConfig();
    if (!initSuccess) {
        return 0;
    }

    //TODO get amount of textures from the gamedata. for debug there's just 1 texture
    for (int i = 0; i < cfg.textureAmount; i++) {
        Texture txt = LoadQRayAsset("assets\\" + std::to_string(i) + ".qrayasset");
        gTextures.push_back(txt);
    }

    LoadWorld("map.txt");

    Win32State win32 = {};

    gWin32 = &win32;

    win32.hInst = hInstance;

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    //char[64] -> std::string -> wchar_t*
    std::string narrowTitle = cfg.title;

    std::wstring title(narrowTitle.begin(), narrowTitle.end());

    wcscpy_s(win32.szTitle, title.c_str());
    wcscpy_s(win32.szWindowClass, L"QRayWindowClass");

    MyRegisterClass(win32, hInstance);

    win32.pixels = new uint32_t[cfg.WINDOW_WIDTH * cfg.WINDOW_HEIGHT];

    if (!InitInstance(win32, hInstance, nCmdShow))
    {
        return FALSE;
    }

    // ==========================================
    // Create framebuffer
    // ==========================================

    ZeroMemory(&win32.bitmapInfo, sizeof(win32.bitmapInfo));

    win32.bitmapInfo.bmiHeader.biSize = sizeof(win32.bitmapInfo.bmiHeader);
    win32.bitmapInfo.bmiHeader.biWidth = cfg.WINDOW_WIDTH;
    win32.bitmapInfo.bmiHeader.biHeight = -cfg.WINDOW_HEIGHT;
    win32.bitmapInfo.bmiHeader.biPlanes = 1;
    win32.bitmapInfo.bmiHeader.biBitCount = 32;
    win32.bitmapInfo.bmiHeader.biCompression = BI_RGB;

    MSG msg = {};

    // ==========================================
    // Main Loop
    // ==========================================

    while (true)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();

        deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();

        lastTime = currentTime;

        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                delete[] win32.pixels;
                DestroyWindow(win32.hWnd);
                return (int)msg.wParam;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Render frame
        Render(win32);

        // Force repaint
        InvalidateRect(win32.hWnd, nullptr, FALSE);
    }
}

ATOM MyRegisterClass(Win32State& win32, HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = {};

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);

    // No automatic background painting
    wcex.hbrBackground = nullptr;

    wcex.lpszClassName = win32.szWindowClass;

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(Win32State& win32, HINSTANCE hInstance, int nCmdShow)
{
    win32.hInst = hInstance;

    //char[64] -> std::string -> wchar_t*
    std::string narrowTitle = cfg.title;

    std::wstring title(narrowTitle.begin(), narrowTitle.end());

    HWND hWnd = CreateWindowW(
        win32.szWindowClass,
        title.c_str(),
        WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        cfg.WINDOW_WIDTH,
        cfg.WINDOW_HEIGHT,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!hWnd)
    {
        return FALSE;
    }

    win32.hWnd = hWnd;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(win32.hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        StretchDIBits(
            hdc,
            0, 0,
            cfg.WINDOW_WIDTH, cfg.WINDOW_HEIGHT,
            0, 0,
            cfg.WINDOW_WIDTH, cfg.WINDOW_HEIGHT,
            gWin32->pixels,
            &gWin32->bitmapInfo,
            DIB_RGB_COLORS,
            SRCCOPY
        );

        auto current = std::chrono::high_resolution_clock::now();

        float deltaTime = std::chrono::duration<float>(current - lastTime).count();

        lastTime = current;

        EndPaint(hWnd, &ps);
    }
    return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}