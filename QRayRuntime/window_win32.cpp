#define NOMINMAX

#include "window_win32.hpp"
#include "config.hpp"
#include <cstdint>
#include "renderer.hpp"
#include "world.hpp"
#include <string>
#include "helperFunctions.hpp"
#include "input.hpp"

static auto lastTime = std::chrono::high_resolution_clock::now();

float deltaTime = 0.0f;

static Win32State* gWin32 = nullptr;

void PutPixel(Framebuffer framebuffer, int x, int y, uint32_t color)
{
    if (x < 0 || x >= framebuffer.width || y < 0 || y >= framebuffer.height)
    {
        return;
    }

    framebuffer.pixels[y * framebuffer.width + x] = color;
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

    for (int i = 0; i < cfg.textureAmount; i++) {
        Texture txt = LoadQRayAsset("assets\\" + std::to_string(i) + ".qrayasset");
        gTextures.push_back(txt);
    }

    for (int i = (cfg.textureAmount - cfg.entityAmount); i < cfg.textureAmount; i++) {
        EntityType entity = LoadQRayEntity("assets\\" + std::to_string(i) + ".qrayentity");
        gEntityTypes.push_back(entity);
    }

    bool worldSuccess = LoadWorld("map.txt");
    if (!worldSuccess) {
        return 0;
    }

    Win32State win32 = {};

    gWin32 = &win32;

    win32.hInst = hInstance;

    win32.framebuffer.width = 640;

    win32.framebuffer.height = 400;

    win32.framebuffer.pixels = new uint32_t[640 * 400];

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    //char[64] -> std::string -> wchar_t*
    std::string narrowTitle = cfg.title;

    std::wstring title(narrowTitle.begin(), narrowTitle.end());

    wcscpy_s(win32.szTitle, title.c_str());
    wcscpy_s(win32.szWindowClass, L"QRayWindowClass");

    MyRegisterClass(win32, hInstance);

    if (!InitInstance(win32, hInstance, nCmdShow))
    {
        return FALSE;
    }

    // ==========================================
    // Create framebuffer
    // ==========================================

    ZeroMemory(&win32.bitmapInfo, sizeof(win32.bitmapInfo));

    win32.bitmapInfo.bmiHeader.biSize = sizeof(win32.bitmapInfo.bmiHeader);
    win32.bitmapInfo.bmiHeader.biWidth = 640;
    win32.bitmapInfo.bmiHeader.biHeight = -400;
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
                delete[] win32.framebuffer.pixels;
                DestroyWindow(win32.hWnd);
                return (int)msg.wParam;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        static float fpsTimer = 0.0f;

        fpsTimer += deltaTime;

        if (fpsTimer >= 0.25f)
        {
            fpsTimer = 0.0f;

            float fps = 1.0f / deltaTime;

            std::string title = narrowTitle + " - FPS: " + std::to_string((int)fps);
            //std::string title = narrowTitle + " - coords: " + std::to_string((int)cfg.playerX) + ", " + std::to_string((int)cfg.playerY);

            SetWindowTextA(win32.hWnd, title.c_str());
        }

        //DOOR ANIMATIONS:
        const float doorSpeed = 1.0f; // open/close speed

        for (Tile& tile : worldWalls)
        {
            if (!tile.isDoor)
                continue;

            Door& door = tile.door;

            if (door.targetOpen)
            {
                door.openTimer -= deltaTime;

                if (!PlayerBetweenDoor(tile, worldWalls[tile.door.indexOfOtherDoorTile])) {
                    if (door.openTimer <= 0.0f)
                    {
                        door.openTimer = 0.0f;
                        door.targetOpen = false; // auto-close trigger
                    }
                }
                
            }

            float direction = door.targetOpen ? 1.0f : -1.0f;

            door.open += direction * doorSpeed * deltaTime;

            if (door.open > 1.0f)
                door.open = 1.0f;

            if (door.open < 0.0f)
                door.open = 0.0f;
        }

        //First process input (and handle movement)
        processInput();

        //process the entity pickups
        processEntities();

        // Render frame
        Render(win32.framebuffer);

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
            gWin32->framebuffer.width, gWin32->framebuffer.height,
            gWin32->framebuffer.pixels,
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