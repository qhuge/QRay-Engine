#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <tchar.h>

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"

#include "editor_main.hpp"
#include "editor_startup.hpp"
#include "editor_viewport.hpp"
#include "config.hpp"
#include <string>
#include "editor_helpers.hpp"
#include <filesystem>
#include "editor_popups_add_tile.hpp"
#include "build.hpp"
#include "editor_popups_error.hpp"
#include "editor_popups_add_entity.hpp"
#include "editor_popups_settings.hpp"

// --------------------
// Globals (minimal)
// --------------------
static HWND g_hWnd = nullptr;
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

int gSelectedTileType = -1;
int gSelectedEntityType = -1;
EditorCreateMode gCreateMode;
int gDoorDirection = 0; //1 == PLACE THE DOOR TILES VERTICALLY, 0 = DOOR TILES HORIZONTALLY

// Forward declare Win32 WndProc
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// --------------------
// DX11 helpers
// --------------------
static void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer = nullptr;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

static void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

// --------------------
// WndProc
// --------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

// --------------------
// Init Editor Window
// --------------------
bool InitEditor(HINSTANCE hInstance, int nCmdShow)
{
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = _T("QRayImGuiEditor");
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClass(&wc);

    g_hWnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        _T("QRay Editor"),
        WS_OVERLAPPEDWINDOW,
        100, 100, 1280, 720,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    ShowWindow(g_hWnd, nCmdShow);

    // --------------------
    // DX11 Init (minimal)
    // --------------------
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 2;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = g_hWnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &sd,
        &g_pSwapChain,
        &g_pd3dDevice,
        nullptr,
        &g_pd3dDeviceContext
    );

    CreateRenderTarget();

    // --------------------
    // ImGui Init
    // --------------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui::GetIO().IniFilename = nullptr;

    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    return true;
}

// --------------------
// Main loop
// --------------------
int RunEditorLoop()
{
    MSG msg;

    while (true)
    {
        while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
                return 0;
        }

        // Start ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        //popups
        DrawAddTilePopup();
        DrawErrorPopup();
        DrawAddEntityPopup();
        DrawSettingsPopup();



        if (gEditorState == EditorState::Startup) {
            DrawStartupWindow(g_hWnd);
        }
        else {
            ImVec2 screen = ImGui::GetIO().DisplaySize;

            float inspectorWidth = 300.0f;

            float menuBarHeight = ImGui::GetFrameHeight();

            //GRID
            ImGui::SetNextWindowPos(ImVec2(0, menuBarHeight));
            ImGui::SetNextWindowSize(ImVec2(screen.x - inspectorWidth, screen.y - menuBarHeight));

            ImGuiWindowFlags flags =
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove;

            ImGui::Begin("Viewport", nullptr, flags);

            DrawViewport();

            ImGui::End();

            //INSPECTOR
            ImGui::SetNextWindowPos(ImVec2(screen.x - inspectorWidth, menuBarHeight));
            ImGui::SetNextWindowSize(ImVec2(inspectorWidth, screen.y - menuBarHeight));

            ImGui::Begin("Inspector", nullptr, flags);

            //TILES
            ImGui::Text("Tile Types");
            ImGui::Separator();

            if (ImGui::Button("+ Add Tile Type"))
            {
                OpenAddTilePopup();
            }

            for (int i = 0; i < save.tileTypes.size(); i++)
            {
                bool selected = (gSelectedTileType == i);

                if (selected)
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.9f, 1.0f));

                if (ImGui::Button(save.tileTypes[i].name))
                {
                    gSelectedTileType = i;
                    SetEditorMode(EditorCreateMode::Tile);
                }

                if (selected)
                    ImGui::PopStyleColor();
            }

            bool spawnSelected = (gCreateMode == EditorCreateMode::Spawnpoint);

            if (spawnSelected)
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.9f, 1.0f));

            if (ImGui::Button("Spawnpoint"))
            {
                SetEditorMode(EditorCreateMode::Spawnpoint);
            }

            if (spawnSelected)
                ImGui::PopStyleColor();

            //ENTITIES:
            ImGui::Spacing();
            ImGui::Text("Entity Types");
            ImGui::Separator();

            if (ImGui::Button("+ Add Entity Type"))
            {
                OpenAddEntityPopup();
            }

            for (int i = 0; i < save.entityTypes.size(); i++)
            {
                bool selected = (gSelectedEntityType == i);

                if (selected)
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.9f, 1.0f));

                if (ImGui::Button(save.entityTypes[i].name))
                {
                    gSelectedEntityType = i;
                    SetEditorMode(EditorCreateMode::Entity);
                }

                if (selected)
                    ImGui::PopStyleColor();
            }


            ImGui::End();


            //TOP BAR
            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Save"))
                    {
                        SaveProject(saveLocation);
                    }

                    if (ImGui::MenuItem("Load"))
                    {
                        std::string file = SelectProjectFile();

                        if (!file.empty())
                        {
                            saveLocation = std::filesystem::path(file).parent_path().string();

                            LoadProject(file, g_hWnd);

                            gEditorState = EditorState::Editing;
                        }
                    }

                    ImGui::Separator();

                    if (ImGui::MenuItem("Build"))
                    {
                        bool ableToBuild = true;

                        if (!save.hasPlacedSpawnpoint) {
                            ShowError("Build failed", "No spawnpoint chosen. Choose a spawnpoint before building the game.");
                            ableToBuild = false;
                        }

                        if (!fileExists(GetExecutableDirectory() + "\\QRayRuntime.exe")) {
                            ShowError("Build failed", "No runtime found. Ensure the runtime is in the same folder as this editor.exe file, and named 'QRayRuntime.exe'");
                            ableToBuild = false;
                        }

                        if (ableToBuild) {
                            
                            std::string buildLocation = SelectFolder();
                            if (!buildLocation.empty())
                            {
                                build(buildLocation);
                            } else {
                                ShowError("Build failed", "Invalid build path");
                            }
                        }
                    }

                    ImGui::Separator();

                    if (ImGui::MenuItem("Exit"))
                    {
                        PostQuitMessage(0);
                    }

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Settings")) {

                    if (ImGui::MenuItem("Project settings")) {
                        OpenSettingsPopup();
                    }

                    ImGui::EndMenu();

                }

                ImGui::EndMainMenuBar();
            }
        }

        // Render
        ImGui::Render();

        float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clearColor);

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0);
    }
}

// --------------------
// Shutdown
// --------------------
void ShutdownEditor()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupRenderTarget();

    if (g_pSwapChain) g_pSwapChain->Release();
    if (g_pd3dDeviceContext) g_pd3dDeviceContext->Release();
    if (g_pd3dDevice) g_pd3dDevice->Release();
}